#include "command/command_dispatcher.hpp"

#include <exception>
#include <stdexcept>

#include "common/string_utils.hpp"
#include "protocol/resp.hpp"

namespace makrounadb::command {

CommandDispatcher::CommandDispatcher(storage::KeyValueStore& store) : store_(store) {}

std::string CommandDispatcher::execute(const std::vector<std::string>& arguments) {
    if (arguments.empty()) {
        return protocol::error("empty command");
    }

    const std::string command = common::to_upper(arguments.front());

    if (command == "PING") {
        if (arguments.size() == 1) {
            return protocol::simple_string("PONG");
        }
        if (arguments.size() == 2) {
            return protocol::bulk_string(arguments[1]);
        }
        return protocol::error("wrong number of arguments for 'PING'");
    }

    if (command == "ECHO") {
        if (arguments.size() != 2) {
            return protocol::error("wrong number of arguments for 'ECHO'");
        }
        return protocol::bulk_string(arguments[1]);
    }

    if (command == "SET") {
        if (arguments.size() != 3) {
            return protocol::error("wrong number of arguments for 'SET'");
        }
        store_.set(arguments[1], arguments[2]);
        return protocol::simple_string("OK");
    }

    if (command == "GET") {
        if (arguments.size() != 2) {
            return protocol::error("wrong number of arguments for 'GET'");
        }
        if (const auto value = store_.get(arguments[1]); value.has_value()) {
            return protocol::bulk_string(*value);
        }
        return protocol::null_bulk_string();
    }

    if (command == "DEL") {
        if (arguments.size() < 2) {
            return protocol::error("wrong number of arguments for 'DEL'");
        }
        long long deleted = 0;
        for (std::size_t index = 1; index < arguments.size(); ++index) {
            deleted += static_cast<long long>(store_.del(arguments[index]));
        }
        return protocol::integer(deleted);
    }

    if (command == "EXISTS") {
        if (arguments.size() < 2) {
            return protocol::error("wrong number of arguments for 'EXISTS'");
        }
        long long count = 0;
        for (std::size_t index = 1; index < arguments.size(); ++index) {
            if (store_.exists(arguments[index])) {
                ++count;
            }
        }
        return protocol::integer(count);
    }

    if (command == "INCR") {
        if (arguments.size() != 2) {
            return protocol::error("wrong number of arguments for 'INCR'");
        }
        try {
            return protocol::integer(store_.incr(arguments[1]));
        } catch (const std::exception&) {
            return protocol::error("value is not an integer or out of range");
        }
    }

    if (command == "EXPIRE") {
        if (arguments.size() != 3) {
            return protocol::error("wrong number of arguments for 'EXPIRE'");
        }

        try {
            std::size_t consumed_characters = 0;
            const long long seconds = std::stoll(arguments[2], &consumed_characters);
            if (consumed_characters != arguments[2].size()) {
                return protocol::error("value is not an integer or out of range");
            }
            return protocol::integer(store_.expire(arguments[1], seconds) ? 1 : 0);
        } catch (const std::exception&) {
            return protocol::error("value is not an integer or out of range");
        }
    }

    if (command == "TTL") {
        if (arguments.size() != 2) {
            return protocol::error("wrong number of arguments for 'TTL'");
        }
        return protocol::integer(store_.ttl(arguments[1]));
    }

    return protocol::error("unknown command");
}

std::size_t CommandDispatcher::run_maintenance() {
    return store_.cleanup_expired();
}

}  // namespace makrounadb::command
