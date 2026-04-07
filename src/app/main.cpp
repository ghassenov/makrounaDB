#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <optional>
#include <string_view>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "command/command_dispatcher.hpp"
#include "common/nl_parser.hpp"
#include "network/server.hpp"
#include "persistence/aof_persistence.hpp"
#include "storage/key_value_store.hpp"

namespace {

makrounadb::network::Server* g_server = nullptr;

void handle_signal(int) {
    if (g_server != nullptr) {
        g_server->stop();
    }
}

void print_banner() {
    std::cout
        << "\n"
        << "██▄  ▄██  ▄▄▄  ▄▄ ▄▄ ▄▄▄▄   ▄▄▄  ▄▄ ▄▄ ▄▄  ▄▄  ▄▄▄  ████▄  █████▄ \n"
        << "██ ▀▀ ██ ██▀██ ██▄█▀ ██▄█▄ ██▀██ ██ ██ ███▄██ ██▀██ ██  ██ ██▄▄██ \n"
        << "██    ██ ██▀██ ██ ██ ██ ██ ▀███▀ ▀███▀ ██ ▀██ ██▀██ ████▀  ██▄▄█▀ \n"
        << "                                                                 \n"
        << "                    ██              ██                      \n"
        << "                  ██              ██                        \n"
        << "                ██        ██    ██                          \n"
        << "                ██      ██      ██                          \n"
        << "                  ██    ██        ██                        \n"
        << "                    ██  ██          ██                      \n"
        << "                    ██    ██        ██                      \n"
        << "                  ██        ██    ██                        \n"
        << "                            ██  ██                          \n"
        << "                          ██                                \n"
        << "                                                            \n"
        << "        ██████        ████████████                          \n"
        << "      ██      ██    ██            ██                        \n"
        << "    ██    ████  ████    ████████    ██              ████    \n"
        << "  ██    ██    ████    ██        ██    ██████      ██    ██  \n"
        << "  ██  ██  ██████    ██    ████    ██    ██  ██  ██        ██\n"
        << "██  ██  ██  ██    ██    ██    ██    ██    ██  ██          ██\n"
        << "████████████████████████████████████████████████████    ██  \n"
        << "  ██████████████████████████████████████      ████    ██    \n"
        << "  ██████████████████████████████████████      ████  ██      \n"
        << "    ██████████████████████████████████      ████    ██      \n"
        << "      ██████████████████████████████    ██████        ██    \n"
        << "        ██████████████████████████  ████████                \n"
        << "            ████████████████████████████                    \n"
        << "\n";
}
void print_help(const char* binary_name) {
    std::cout
        << "Usage:\n"
        << "  " << binary_name << " [--port <1-65535>] [--aof <path>] [--no-banner]\n"
        << "  " << binary_name << " --nl-shell [--host <ip>] [--port <1-65535>]\n\n"
        << "Options:\n"
        << "  --port <port>    TCP port to listen on (default: 6379)\n"
        << "  --host <ip>      Host for --nl-shell mode (default: 127.0.0.1)\n"
        << "  --aof <path>     AOF file path (default: data/appendonly.aof)\n"
        << "  --nl-shell       Start natural language shell client mode\n"
        << "  --no-banner      Disable startup ASCII banner\n"
        << "  -h, --help       Show this help message\n";
}

std::string encode_resp_array(const std::vector<std::string>& arguments) {
    std::string encoded = "*" + std::to_string(arguments.size()) + "\r\n";
    for (const auto& argument : arguments) {
        encoded += "$" + std::to_string(argument.size()) + "\r\n";
        encoded += argument;
        encoded += "\r\n";
    }
    return encoded;
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

std::optional<std::string> read_line_crlf(int socket_fd) {
    std::string line;
    char character = '\0';
    while (true) {
        const ssize_t bytes = ::recv(socket_fd, &character, 1, 0);
        if (bytes <= 0) {
            return std::nullopt;
        }
        line.push_back(character);
        if (line.size() >= 2 && line[line.size() - 2] == '\r' && line[line.size() - 1] == '\n') {
            return line;
        }
    }
}

std::optional<std::string> read_exact(int socket_fd, std::size_t count) {
    std::string output;
    output.resize(count);

    std::size_t total = 0;
    while (total < count) {
        const ssize_t bytes = ::recv(socket_fd, output.data() + total, count - total, 0);
        if (bytes <= 0) {
            return std::nullopt;
        }
        total += static_cast<std::size_t>(bytes);
    }

    return output;
}

std::optional<std::string> read_resp_reply(int socket_fd) {
    const auto header = read_line_crlf(socket_fd);
    if (!header.has_value() || header->empty()) {
        return std::nullopt;
    }

    const char prefix = header->front();
    if (prefix == '+' || prefix == '-' || prefix == ':') {
        return header;
    }

    if (prefix == '$') {
        const std::string length_text = header->substr(1, header->size() - 3);
        const long long length = std::strtoll(length_text.c_str(), nullptr, 10);
        if (length < 0) {
            return header;
        }
        const auto payload = read_exact(socket_fd, static_cast<std::size_t>(length) + 2);
        if (!payload.has_value()) {
            return std::nullopt;
        }
        return *header + *payload;
    }

    return std::nullopt;
}

std::string pretty_reply(std::string_view reply) {
    if (reply.empty()) {
        return "<empty reply>";
    }

    const char prefix = reply.front();
    if (prefix == '+') {
        return std::string(reply.substr(1, reply.size() - 3));
    }
    if (prefix == '-') {
        return std::string(reply.substr(1, reply.size() - 3));
    }
    if (prefix == ':') {
        return std::string(reply.substr(1, reply.size() - 3));
    }
    if (prefix == '$') {
        const std::size_t header_end = reply.find("\r\n");
        if (header_end == std::string_view::npos) {
            return std::string(reply);
        }
        const long long length = std::strtoll(std::string(reply.substr(1, header_end - 1)).c_str(), nullptr, 10);
        if (length < 0) {
            return "(nil)";
        }
        return std::string(reply.substr(header_end + 2, static_cast<std::size_t>(length)));
    }

    return std::string(reply);
}

int run_nl_shell(const std::string& host, std::uint16_t port) {
    const int socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        std::cerr << "error: unable to create socket\n";
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (::inet_pton(AF_INET, host.c_str(), &address.sin_addr) <= 0) {
        std::cerr << "error: invalid host\n";
        ::close(socket_fd);
        return 1;
    }

    if (::connect(socket_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        std::cerr << "error: could not connect to " << host << ':' << port << "\n";
        ::close(socket_fd);
        return 1;
    }

    std::cout << "Connected to makrounaDB at " << host << ':' << port << "\n";
    std::cout << "Type natural language commands (type 'quit' to exit).\n";

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "quit" || line == "exit") {
            break;
        }

        const auto command = makrounadb::common::parse_natural_language_command(line);
        if (!command.has_value()) {
            std::cout << "I did not understand that. Example: set user to ghassen\n";
            continue;
        }

        const std::string payload = encode_resp_array(*command);
        if (!send_all(socket_fd, payload)) {
            std::cerr << "error: failed to send command\n";
            ::close(socket_fd);
            return 1;
        }

        const auto reply = read_resp_reply(socket_fd);
        if (!reply.has_value()) {
            std::cerr << "error: failed to read reply\n";
            ::close(socket_fd);
            return 1;
        }

        std::cout << pretty_reply(*reply) << "\n";
    }

    ::close(socket_fd);
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    std::uint16_t port = 6379;
    std::string aof_path = "data/appendonly.aof";
    std::string host = "127.0.0.1";
    bool show_banner = true;
    bool nl_shell_mode = false;

    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];

        if (argument == "-h" || argument == "--help") {
            print_help(argv[0]);
            return 0;
        }

        if (argument == "--no-banner") {
            show_banner = false;
            continue;
        }

        if (argument == "--nl-shell") {
            nl_shell_mode = true;
            continue;
        }

        if (argument == "--host") {
            if (index + 1 >= argc) {
                std::cerr << "error: --host expects a value\n";
                return 1;
            }
            host = argv[++index];
            continue;
        }

        if (argument == "--port") {
            if (index + 1 >= argc) {
                std::cerr << "error: --port expects a value\n";
                return 1;
            }
            const long parsed_port = std::strtol(argv[++index], nullptr, 10);
            if (parsed_port <= 0 || parsed_port > 65535) {
                std::cerr << "error: invalid port\n";
                return 1;
            }
            port = static_cast<std::uint16_t>(parsed_port);
            continue;
        }

        if (argument == "--aof") {
            if (index + 1 >= argc) {
                std::cerr << "error: --aof expects a value\n";
                return 1;
            }
            aof_path = argv[++index];
            continue;
        }

        if (!argument.empty() && argument[0] == '-') {
            std::cerr << "error: unknown option: " << argument << "\n";
            return 1;
        }

        const long parsed_port = std::strtol(argument.c_str(), nullptr, 10);
        if (parsed_port > 0 && parsed_port <= 65535) {
            port = static_cast<std::uint16_t>(parsed_port);
        } else {
            aof_path = argument;
        }
    }

    if (nl_shell_mode) {
        return run_nl_shell(host, port);
    }

    makrounadb::storage::KeyValueStore store;
    makrounadb::command::CommandDispatcher dispatcher(store);
    makrounadb::persistence::AofPersistence persistence(aof_path);

    if (!persistence.load_and_replay(dispatcher)) {
        std::cerr << "warning: AOF replay failed for " << aof_path << std::endl;
    }

    if (!persistence.open()) {
        std::cerr << "warning: unable to open AOF file " << aof_path << std::endl;
    }

    makrounadb::network::Server server(dispatcher, port, &persistence);
    g_server = &server;

    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    if (show_banner) {
        print_banner();
    }

    std::cout << "makrounaDB listening on port " << port << " (AOF: " << aof_path << ")" << std::endl;

    server.run();

    return 0;
}
