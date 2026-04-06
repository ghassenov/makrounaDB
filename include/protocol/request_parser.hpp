#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace makrounadb::protocol {

enum class ParseStatus {
    kComplete,
    kIncomplete,
    kError,
};

struct ParseResult {
    ParseStatus status;
    std::vector<std::string> arguments;
    std::size_t bytes_consumed = 0;
    std::string error_message;
};

ParseResult parse_request(std::string_view buffer);

}  // namespace makrounadb::protocol
