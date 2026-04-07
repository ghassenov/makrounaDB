#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cassert>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

namespace {

std::string read_exact(int socket_fd, std::size_t count) {
    std::string output;
    output.resize(count);

    std::size_t total_read = 0;
    while (total_read < count) {
        const ssize_t bytes = ::recv(socket_fd, output.data() + total_read, count - total_read, 0);
        if (bytes <= 0) {
            return {};
        }
        total_read += static_cast<std::size_t>(bytes);
    }

    return output;
}

std::string read_line_crlf(int socket_fd) {
    std::string line;
    char character = '\0';

    while (true) {
        const ssize_t bytes = ::recv(socket_fd, &character, 1, 0);
        if (bytes <= 0) {
            return {};
        }

        line.push_back(character);
        if (line.size() >= 2 && line[line.size() - 2] == '\r' && line[line.size() - 1] == '\n') {
            return line;
        }
    }
}

std::string read_resp_reply(int socket_fd) {
    const std::string header = read_line_crlf(socket_fd);
    if (header.empty()) {
        return {};
    }

    const char type = header.front();
    if (type == '+' || type == '-' || type == ':') {
        return header;
    }

    if (type == '$') {
        const std::string length_str = header.substr(1, header.size() - 3);
        const long long payload_len = std::stoll(length_str);
        if (payload_len < 0) {
            return header;
        }

        const std::string payload = read_exact(socket_fd, static_cast<std::size_t>(payload_len) + 2);
        if (payload.empty()) {
            return {};
        }
        return header + payload;
    }

    return {};
}

bool send_all(int socket_fd, const std::string& payload) {
    std::size_t total_sent = 0;
    while (total_sent < payload.size()) {
        const ssize_t bytes = ::send(socket_fd, payload.data() + total_sent, payload.size() - total_sent, 0);
        if (bytes <= 0) {
            return false;
        }
        total_sent += static_cast<std::size_t>(bytes);
    }
    return true;
}

std::string encode_command(const std::vector<std::string>& arguments) {
    std::string encoded = "*" + std::to_string(arguments.size()) + "\r\n";
    for (const auto& argument : arguments) {
        encoded += "$" + std::to_string(argument.size()) + "\r\n";
        encoded += argument;
        encoded += "\r\n";
    }
    return encoded;
}

pid_t start_server_process(const std::string& server_path, int port, const std::string& aof_path) {
    const pid_t pid = ::fork();
    if (pid == 0) {
        const std::string port_text = std::to_string(port);
        execl(server_path.c_str(), server_path.c_str(), port_text.c_str(), aof_path.c_str(), nullptr);
        _exit(1);
    }
    return pid;
}

int connect_with_retry(int port, int attempts, std::chrono::milliseconds delay) {
    for (int attempt = 0; attempt < attempts; ++attempt) {
        const int socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            continue;
        }

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_port = htons(static_cast<uint16_t>(port));
        ::inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

        if (::connect(socket_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == 0) {
            return socket_fd;
        }

        ::close(socket_fd);
        std::this_thread::sleep_for(delay);
    }

    return -1;
}

void stop_server_process(pid_t pid) {
    if (pid <= 0) {
        return;
    }

    ::kill(pid, SIGTERM);
    int status = 0;
    ::waitpid(pid, &status, 0);
}

}  // namespace

int main(int argc, char** argv) {
    assert(argc >= 2);
    const std::string server_path = argv[1];

    const int port = 17079;
    const std::filesystem::path aof_path = std::filesystem::temp_directory_path() / "makrounadb_socket_test.aof";
    std::filesystem::remove(aof_path);

    const pid_t first_pid = start_server_process(server_path, port, aof_path.string());
    assert(first_pid > 0);

    int client_fd = connect_with_retry(port, 40, std::chrono::milliseconds(50));
    assert(client_fd >= 0);

    assert(send_all(client_fd, encode_command({"PING"})));
    assert(read_resp_reply(client_fd) == "+PONG\r\n");

    assert(send_all(client_fd, encode_command({"SET", "persisted", "42"})));
    assert(read_resp_reply(client_fd) == "+OK\r\n");

    assert(send_all(client_fd, encode_command({"EXPIRE", "persisted", "5"})));
    assert(read_resp_reply(client_fd) == ":1\r\n");

    assert(send_all(client_fd, encode_command({"GET", "persisted"})));
    assert(read_resp_reply(client_fd) == "$2\r\n42\r\n");

    ::close(client_fd);
    stop_server_process(first_pid);

    const pid_t second_pid = start_server_process(server_path, port, aof_path.string());
    assert(second_pid > 0);

    client_fd = connect_with_retry(port, 40, std::chrono::milliseconds(50));
    assert(client_fd >= 0);

    assert(send_all(client_fd, encode_command({"GET", "persisted"})));
    assert(read_resp_reply(client_fd) == "$2\r\n42\r\n");

    ::close(client_fd);
    stop_server_process(second_pid);

    std::filesystem::remove(aof_path);
    return 0;
}
