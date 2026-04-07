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

    // LINGUINE: increment by N (like INCRBY)
    if (command == "LINGUINE") {
        if (arguments.size() != 3) {
            return protocol::error("wrong number of arguments for 'LINGUINE'");
        }
        try {
            std::size_t consumed = 0;
            const long long amount = std::stoll(arguments[2], &consumed);
            if (consumed != arguments[2].size()) {
                return protocol::error("value is not an integer or out of range");
            }
            return protocol::integer(store_.incrby(arguments[1], amount));
        } catch (const std::exception&) {
            return protocol::error("value is not an integer or out of range");
        }
    }

    // RIGATONI: decrement by 1 (like DECR)
    if (command == "RIGATONI") {
        if (arguments.size() != 2) {
            return protocol::error("wrong number of arguments for 'RIGATONI'");
        }
        try {
            return protocol::integer(store_.decr(arguments[1]));
        } catch (const std::exception&) {
            return protocol::error("value is not an integer or out of range");
        }
    }

    // VERMICELLI: decrement by N (like DECRBY)
    if (command == "VERMICELLI") {
        if (arguments.size() != 3) {
            return protocol::error("wrong number of arguments for 'VERMICELLI'");
        }
        try {
            std::size_t consumed = 0;
            const long long amount = std::stoll(arguments[2], &consumed);
            if (consumed != arguments[2].size()) {
                return protocol::error("value is not an integer or out of range");
            }
            return protocol::integer(store_.decrby(arguments[1], amount));
        } catch (const std::exception&) {
            return protocol::error("value is not an integer or out of range");
        }
    }

    // SPAGHETTI: string length (like STRLEN)
    if (command == "SPAGHETTI") {
        if (arguments.size() != 2) {
            return protocol::error("wrong number of arguments for 'SPAGHETTI'");
        }
        return protocol::integer(store_.strlen(arguments[1]));
    }

    // PENNE: set if not exists (like SETNX)
    if (command == "PENNE") {
        if (arguments.size() != 3) {
            return protocol::error("wrong number of arguments for 'PENNE'");
        }
        return protocol::integer(store_.setnx(arguments[1], arguments[2]) ? 1 : 0);
    }

    // ALDENTE: remove TTL from key (like PERSIST)
    if (command == "ALDENTE") {
        if (arguments.size() != 2) {
            return protocol::error("wrong number of arguments for 'ALDENTE'");
        }
        return protocol::integer(store_.persist(arguments[1]) ? 1 : 0);
    }

    // FARFALLE: get value then delete key (like GETDEL)
    if (command == "FARFALLE") {
        if (arguments.size() != 2) {
            return protocol::error("wrong number of arguments for 'FARFALLE'");
        }
        if (const auto value = store_.getdel(arguments[1]); value.has_value()) {
            return protocol::bulk_string(*value);
        }
        return protocol::null_bulk_string();
    }

    // LASAGNA: rename a key (like RENAME)
    if (command == "LASAGNA") {
        if (arguments.size() != 3) {
            return protocol::error("wrong number of arguments for 'LASAGNA'");
        }
        if (!store_.rename(arguments[1], arguments[2])) {
            return protocol::error("no such key");
        }
        return protocol::simple_string("OK");
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
