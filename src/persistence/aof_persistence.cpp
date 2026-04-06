#include "persistence/aof_persistence.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "command/command_dispatcher.hpp"
#include "common/string_utils.hpp"
#include "protocol/request_parser.hpp"

namespace makrounadb::persistence {

AofPersistence::AofPersistence(std::string file_path, std::chrono::seconds fsync_interval)
    : file_path_(std::move(file_path)),
      fsync_interval_(fsync_interval),
      next_sync_(std::chrono::steady_clock::now() + fsync_interval) {}

bool AofPersistence::open() {
    std::error_code error_code;
    const std::filesystem::path path(file_path_);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path(), error_code);
    }

    output_.open(file_path_, std::ios::out | std::ios::app | std::ios::binary);
    return output_.is_open();
}

bool AofPersistence::load_and_replay(command::CommandDispatcher& dispatcher) {
    std::ifstream input(file_path_, std::ios::in | std::ios::binary);
    if (!input.is_open()) {
        return true;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    std::string content = buffer.str();

    std::size_t cursor = 0;
    while (cursor < content.size()) {
        const auto parsed = protocol::parse_request(std::string_view(content).substr(cursor));
        if (parsed.status == protocol::ParseStatus::kIncomplete) {
            return false;
        }
        if (parsed.status == protocol::ParseStatus::kError) {
            return false;
        }

        dispatcher.execute(parsed.arguments);
        cursor += parsed.bytes_consumed;
    }

    return true;
}

void AofPersistence::append_if_write(const std::vector<std::string>& arguments, const std::string& response) {
    if (!output_.is_open()) {
        return;
    }

    if (!is_write_command(arguments)) {
        return;
    }

    if (response.rfind("-ERR", 0) == 0) {
        return;
    }

    output_ << encode_as_resp_array(arguments);
}

void AofPersistence::periodic_sync() {
    if (!output_.is_open()) {
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    if (now < next_sync_) {
        return;
    }

    output_.flush();
    next_sync_ = now + fsync_interval_;
}

void AofPersistence::flush() {
    if (output_.is_open()) {
        output_.flush();
    }
}

bool AofPersistence::is_write_command(const std::vector<std::string>& arguments) {
    if (arguments.empty()) {
        return false;
    }

    const std::string command = common::to_upper(arguments.front());
    return command == "SET" || command == "DEL" || command == "INCR" || command == "EXPIRE";
}

std::string AofPersistence::encode_as_resp_array(const std::vector<std::string>& arguments) {
    std::string encoded = "*" + std::to_string(arguments.size()) + "\r\n";
    for (const auto& argument : arguments) {
        encoded += "$" + std::to_string(argument.size()) + "\r\n";
        encoded += argument;
        encoded += "\r\n";
    }
    return encoded;
}

}  // namespace makrounadb::persistence
