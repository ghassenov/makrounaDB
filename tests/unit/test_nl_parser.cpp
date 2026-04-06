#include <cassert>

#include "common/nl_parser.hpp"

int main() {
    using makrounadb::common::parse_natural_language_command;

    {
        const auto parsed = parse_natural_language_command("ping");
        assert(parsed.has_value());
        assert((*parsed)[0] == "PING");
    }

    {
        const auto parsed = parse_natural_language_command("set user to ghassen");
        assert(parsed.has_value());
        assert(parsed->size() == 3);
        assert((*parsed)[0] == "SET");
        assert((*parsed)[1] == "user");
        assert((*parsed)[2] == "ghassen");
    }

    {
        const auto parsed = parse_natural_language_command("what is user?");
        assert(parsed.has_value());
        assert((*parsed)[0] == "GET");
        assert((*parsed)[1] == "user");
    }

    {
        const auto parsed = parse_natural_language_command("increment counter");
        assert(parsed.has_value());
        assert((*parsed)[0] == "INCR");
        assert((*parsed)[1] == "counter");
    }

    {
        const auto parsed = parse_natural_language_command("expire user in 15 seconds");
        assert(parsed.has_value());
        assert((*parsed)[0] == "EXPIRE");
        assert((*parsed)[1] == "user");
        assert((*parsed)[2] == "15");
    }

    {
        const auto parsed = parse_natural_language_command("ttl user");
        assert(parsed.has_value());
        assert((*parsed)[0] == "TTL");
        assert((*parsed)[1] == "user");
    }

    {
        const auto parsed = parse_natural_language_command("something unsupported");
        assert(!parsed.has_value());
    }

    return 0;
}
