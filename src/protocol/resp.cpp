#include "protocol/resp.hpp"

namespace makrounadb::protocol {

std::string simple_string(const std::string& value) {
    return "+" + value + "\r\n";
}

std::string bulk_string(const std::string& value) {
    return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
}

std::string null_bulk_string() {
    return "$-1\r\n";
}

std::string integer(long long value) {
    return ":" + std::to_string(value) + "\r\n";
}

std::string error(const std::string& message) {
    return "-ERR " + message + "\r\n";
}

}  // namespace makrounadb::protocol
