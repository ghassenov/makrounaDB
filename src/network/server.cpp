#include "network/server.hpp"

#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#include "protocol/resp.hpp"
#include "protocol/request_parser.hpp"

namespace makrounadb::network {
namespace {

constexpr int kReadBufferSize = 4096;

bool send_all(int socket_fd, const std::string& payload) {
    std::size_t total_sent = 0;
    while (total_sent < payload.size()) {
        const ssize_t sent = ::send(
            socket_fd,
            payload.data() + total_sent,
            payload.size() - total_sent,
            0
        );
        if (sent < 0) {
            if (errno == EINTR) {
                continue;
            }
            return false;
        }
        if (sent == 0) {
            return false;
        }
        total_sent += static_cast<std::size_t>(sent);
    }
    return true;
}

}  // namespace

Server::Server(
    command::CommandDispatcher& dispatcher,
    std::uint16_t port,
    persistence::AofPersistence* persistence
)
    : dispatcher_(dispatcher), persistence_(persistence), port_(port) {}

bool Server::setup_listener() {
    listener_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listener_fd_ < 0) {
        return false;
    }

    int reuse_addr = 1;
    if (::setsockopt(listener_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) < 0) {
        ::close(listener_fd_);
        listener_fd_ = -1;
        return false;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port_);

    if (::bind(listener_fd_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        ::close(listener_fd_);
        listener_fd_ = -1;
        return false;
    }

    if (::listen(listener_fd_, SOMAXCONN) < 0) {
        ::close(listener_fd_);
        listener_fd_ = -1;
        return false;
    }

    return true;
}

void Server::run() {
    if (running_.load()) {
        return;
    }

    if (!setup_listener()) {
        return;
    }

    running_.store(true);
    event_loop();

    if (listener_fd_ >= 0) {
        ::close(listener_fd_);
        listener_fd_ = -1;
    }
}

void Server::stop() {
    running_.store(false);

    if (listener_fd_ >= 0) {
        ::shutdown(listener_fd_, SHUT_RDWR);
    }

    if (persistence_ != nullptr) {
        persistence_->flush();
    }
}

bool Server::is_running() const {
    return running_.load();
}

void Server::event_loop() {
    std::vector<int> clients;
    std::unordered_map<int, std::string> client_buffers;

    while (running_.load()) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(listener_fd_, &read_fds);

        int max_fd = listener_fd_;
        for (const int client_fd : clients) {
            FD_SET(client_fd, &read_fds);
            max_fd = std::max(max_fd, client_fd);
        }

        timeval timeout{};
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        const int selected = ::select(max_fd + 1, &read_fds, nullptr, nullptr, &timeout);
        if (selected < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }

        dispatcher_.run_maintenance();
        if (persistence_ != nullptr) {
            persistence_->periodic_sync();
        }

        if (FD_ISSET(listener_fd_, &read_fds)) {
            const int client_fd = ::accept(listener_fd_, nullptr, nullptr);
            if (client_fd >= 0) {
                clients.push_back(client_fd);
                client_buffers.emplace(client_fd, std::string{});
            }
        }

        std::vector<int> disconnected;
        char read_buffer[kReadBufferSize];

        for (const int client_fd : clients) {
            if (!FD_ISSET(client_fd, &read_fds)) {
                continue;
            }

            const ssize_t bytes_read = ::recv(client_fd, read_buffer, sizeof(read_buffer), 0);
            if (bytes_read <= 0) {
                disconnected.push_back(client_fd);
                continue;
            }

            std::string& request_buffer = client_buffers[client_fd];
            request_buffer.append(read_buffer, static_cast<std::size_t>(bytes_read));

            while (true) {
                const protocol::ParseResult parsed = protocol::parse_request(request_buffer);
                if (parsed.status == protocol::ParseStatus::kIncomplete) {
                    break;
                }

                std::string response;
                if (parsed.status == protocol::ParseStatus::kError) {
                    response = protocol::error(parsed.error_message);
                } else {
                    response = dispatcher_.execute(parsed.arguments);
                    if (persistence_ != nullptr) {
                        persistence_->append_if_write(parsed.arguments, response);
                    }
                }

                if (!send_all(client_fd, response)) {
                    disconnected.push_back(client_fd);
                    break;
                }

                const std::size_t consumed = std::min(parsed.bytes_consumed, request_buffer.size());
                request_buffer.erase(0, consumed);
            }
        }

        for (const int client_fd : disconnected) {
            ::close(client_fd);
            client_buffers.erase(client_fd);
            clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());
        }
    }

    for (const int client_fd : clients) {
        ::close(client_fd);
    }
}

}  // namespace makrounadb::network
