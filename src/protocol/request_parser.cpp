#include "protocol/request_parser.hpp"

#include <charconv>
#include <cstddef>
#include <limits>
#include <sstream>

namespace makrounadb::protocol {
namespace {

bool parse_integer(std::string_view value, long long& parsed) {
    if (value.empty()) {
        return false;
    }

    const char* begin = value.data();
    const char* end = value.data() + value.size();
    auto [pointer, error_code] = std::from_chars(begin, end, parsed);
    return error_code == std::errc() && pointer == end;
}

std::size_t find_crlf(std::string_view buffer, std::size_t offset) {
    const std::size_t position = buffer.find("\r\n", offset);
    return position;
}

ParseResult parse_inline(std::string_view buffer) {
    const std::size_t line_end = find_crlf(buffer, 0);
    if (line_end == std::string_view::npos) {
        return {.status = ParseStatus::kIncomplete};
    }

    std::vector<std::string> arguments;
    std::istringstream input(std::string(buffer.substr(0, line_end)));
    for (std::string token; input >> token;) {
        arguments.push_back(token);
    }

    if (arguments.empty()) {
        return {
            .status = ParseStatus::kError,
            .bytes_consumed = line_end + 2,
            .error_message = "empty inline command",
        };
    }

    return {
        .status = ParseStatus::kComplete,
        .arguments = std::move(arguments),
        .bytes_consumed = line_end + 2,
    };
}

ParseResult parse_array(std::string_view buffer) {
    const std::size_t header_end = find_crlf(buffer, 1);
    if (header_end == std::string_view::npos) {
        return {.status = ParseStatus::kIncomplete};
    }

    long long argument_count = 0;
    if (!parse_integer(buffer.substr(1, header_end - 1), argument_count) || argument_count < 0) {
        return {
            .status = ParseStatus::kError,
            .bytes_consumed = header_end + 2,
            .error_message = "invalid multibulk length",
        };
    }

    std::size_t cursor = header_end + 2;
    std::vector<std::string> arguments;
    arguments.reserve(static_cast<std::size_t>(argument_count));

    for (long long index = 0; index < argument_count; ++index) {
        if (cursor >= buffer.size()) {
            return {.status = ParseStatus::kIncomplete};
        }

        if (buffer[cursor] != '$') {
            return {
                .status = ParseStatus::kError,
                .bytes_consumed = cursor + 1,
                .error_message = "expected bulk string",
            };
        }

        const std::size_t bulk_len_end = find_crlf(buffer, cursor + 1);
        if (bulk_len_end == std::string_view::npos) {
            return {.status = ParseStatus::kIncomplete};
        }

        long long bulk_len = 0;
        if (!parse_integer(buffer.substr(cursor + 1, bulk_len_end - (cursor + 1)), bulk_len) || bulk_len < 0) {
            return {
                .status = ParseStatus::kError,
                .bytes_consumed = bulk_len_end + 2,
                .error_message = "invalid bulk length",
            };
        }

        const auto remaining = buffer.size() - (bulk_len_end + 2);
        if (static_cast<unsigned long long>(bulk_len) + 2ULL > remaining) {
            return {.status = ParseStatus::kIncomplete};
        }

        const std::size_t data_start = bulk_len_end + 2;
        const std::size_t data_end = data_start + static_cast<std::size_t>(bulk_len);

        if (buffer.substr(data_end, 2) != "\r\n") {
            return {
                .status = ParseStatus::kError,
                .bytes_consumed = data_end,
                .error_message = "bulk string missing CRLF terminator",
            };
        }

        arguments.emplace_back(buffer.substr(data_start, static_cast<std::size_t>(bulk_len)));
        cursor = data_end + 2;
    }

    return {
        .status = ParseStatus::kComplete,
        .arguments = std::move(arguments),
        .bytes_consumed = cursor,
    };
}

}  // namespace

ParseResult parse_request(std::string_view buffer) {
    if (buffer.empty()) {
        return {.status = ParseStatus::kIncomplete};
    }

    if (buffer[0] == '*') {
        return parse_array(buffer);
    }

    return parse_inline(buffer);
}

}  // namespace makrounadb::protocol
