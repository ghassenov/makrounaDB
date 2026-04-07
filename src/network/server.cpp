#include "network/server.hpp"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

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

#ifdef _WIN32
using SocketLength = int;
constexpr SocketHandle kInvalidSocket = INVALID_SOCKET;

int last_socket_error() {
    return WSAGetLastError();
}

bool is_interrupted_error(const int error_code) {
    return error_code == WSAEINTR;
}

void close_socket(const SocketHandle socket_fd) {
    if (socket_fd != INVALID_SOCKET) {
        ::closesocket(socket_fd);
    }
}
#else
using SocketLength = socklen_t;
constexpr SocketHandle kInvalidSocket = -1;

int last_socket_error() {
    return errno;
}

bool is_interrupted_error(const int error_code) {
    return error_code == EINTR;
}

void close_socket(const SocketHandle socket_fd) {
    if (socket_fd >= 0) {
        ::close(socket_fd);
    }
}
#endif

bool send_all(const SocketHandle socket_fd, const std::string& payload) {
    std::size_t total_sent = 0;
    while (total_sent < payload.size()) {
#ifdef _WIN32
        const int sent = ::send(
            socket_fd,
            payload.data() + total_sent,
            static_cast<int>(payload.size() - total_sent),
            0
        );
#else
        const ssize_t sent = ::send(
            socket_fd,
            payload.data() + total_sent,
            payload.size() - total_sent,
            0
        );
#endif
        if (sent < 0) {
            if (is_interrupted_error(last_socket_error())) {
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
    if (listener_fd_ == kInvalidSocket) {
        return false;
    }

    int reuse_addr = 1;
    if (::setsockopt(listener_fd_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse_addr), sizeof(reuse_addr)) < 0) {
        close_socket(listener_fd_);
        listener_fd_ = kInvalidSocket;
        return false;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port_);

    if (::bind(listener_fd_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        close_socket(listener_fd_);
        listener_fd_ = kInvalidSocket;
        return false;
    }

    if (::listen(listener_fd_, SOMAXCONN) < 0) {
        close_socket(listener_fd_);
        listener_fd_ = kInvalidSocket;
        return false;
    }

    return true;
}

void Server::run() {
    if (running_.load()) {
        return;
    }

#ifdef _WIN32
    WSADATA wsa_data{};
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return;
    }
#endif

    if (!setup_listener()) {
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    running_.store(true);
    event_loop();

    if (listener_fd_ != kInvalidSocket) {
        close_socket(listener_fd_);
        listener_fd_ = kInvalidSocket;
    }

#ifdef _WIN32
    WSACleanup();
#endif
}

void Server::stop() {
    running_.store(false);

    if (listener_fd_ != kInvalidSocket) {
#ifdef _WIN32
        ::shutdown(listener_fd_, SD_BOTH);
#else
        ::shutdown(listener_fd_, SHUT_RDWR);
#endif
    }

    if (persistence_ != nullptr) {
        persistence_->flush();
    }
}

bool Server::is_running() const {
    return running_.load();
}

void Server::event_loop() {
    std::vector<SocketHandle> clients;
    std::unordered_map<SocketHandle, std::string> client_buffers;

    while (running_.load()) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(listener_fd_, &read_fds);

        SocketHandle max_fd = listener_fd_;
        for (const SocketHandle client_fd : clients) {
            FD_SET(client_fd, &read_fds);
            max_fd = std::max(max_fd, client_fd);
        }

        timeval timeout{};
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        const int selected = ::select(static_cast<int>(max_fd) + 1, &read_fds, nullptr, nullptr, &timeout);
        if (selected < 0) {
            if (is_interrupted_error(last_socket_error())) {
                continue;
            }
            break;
        }

        dispatcher_.run_maintenance();
        if (persistence_ != nullptr) {
            persistence_->periodic_sync();
        }

        if (FD_ISSET(listener_fd_, &read_fds)) {
            const SocketHandle client_fd = ::accept(listener_fd_, nullptr, nullptr);
            if (client_fd != kInvalidSocket) {
                clients.push_back(client_fd);
                client_buffers.emplace(client_fd, std::string{});
            }
        }

        std::vector<SocketHandle> disconnected;
        char read_buffer[kReadBufferSize];

        for (const SocketHandle client_fd : clients) {
            if (!FD_ISSET(client_fd, &read_fds)) {
                continue;
            }

#ifdef _WIN32
            const int bytes_read = ::recv(client_fd, read_buffer, static_cast<int>(sizeof(read_buffer)), 0);
#else
            const ssize_t bytes_read = ::recv(client_fd, read_buffer, sizeof(read_buffer), 0);
#endif
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

        for (const SocketHandle client_fd : disconnected) {
            close_socket(client_fd);
            client_buffers.erase(client_fd);
            clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());
        }
    }

    for (const SocketHandle client_fd : clients) {
        close_socket(client_fd);
    }
}

}  // namespace makrounadb::network
