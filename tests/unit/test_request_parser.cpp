#include <cassert>

#include "protocol/request_parser.hpp"

int main() {
    using makrounadb::protocol::ParseStatus;
    using makrounadb::protocol::parse_request;

    {
        const auto result = parse_request("PING\r\n");
        assert(result.status == ParseStatus::kComplete);
        assert(result.arguments.size() == 1);
        assert(result.arguments[0] == "PING");
        assert(result.bytes_consumed == 6);
    }

    {
        const auto result = parse_request("*2\r\n$4\r\nECHO\r\n$5\r\nhello\r\n");
        assert(result.status == ParseStatus::kComplete);
        assert(result.arguments.size() == 2);
        assert(result.arguments[0] == "ECHO");
        assert(result.arguments[1] == "hello");
    }

    {
        const auto result = parse_request("*2\r\n$4\r\nECHO\r\n$5\r\nhel");
        assert(result.status == ParseStatus::kIncomplete);
    }

    {
        const auto result = parse_request("*x\r\n");
        assert(result.status == ParseStatus::kError);
    }

    return 0;
}
