#include "common/nl_parser.hpp"

#include <algorithm>
#include <cctype>
#include <regex>

namespace makrounadb::common {
namespace {

std::string trim(const std::string& value) {
    std::size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    std::size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}

std::string to_lower(std::string value) {
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char character) { return static_cast<char>(std::tolower(character)); }
    );
    return value;
}

}  // namespace

std::optional<std::vector<std::string>> parse_natural_language_command(const std::string& input) {
    const std::string normalized = trim(input);
    if (normalized.empty()) {
        return std::nullopt;
    }

    const std::string lower = to_lower(normalized);
    std::smatch match;

    if (std::regex_match(lower, std::regex("^(ping|are you alive|hello)$"))) {
        return std::vector<std::string>{"PING"};
    }

    if (std::regex_match(normalized, match, std::regex("^echo\\s+(.+)$", std::regex::icase))) {
        return std::vector<std::string>{"ECHO", trim(match[1].str())};
    }

    if (std::regex_match(normalized, match, std::regex("^(set|remember)\\s+([^\\s]+)\\s+(to|as)\\s+(.+)$", std::regex::icase))) {
        return std::vector<std::string>{"SET", match[2].str(), trim(match[4].str())};
    }

    if (std::regex_match(normalized, match, std::regex("^(get|show)\\s+([^\\s]+)$", std::regex::icase))) {
        return std::vector<std::string>{"GET", match[2].str()};
    }

    if (std::regex_match(normalized, match, std::regex("^what\\s+is\\s+([^\\s\\?]+)\\??$", std::regex::icase))) {
        return std::vector<std::string>{"GET", match[1].str()};
    }

    if (std::regex_match(normalized, match, std::regex("^(delete|remove)\\s+([^\\s]+)$", std::regex::icase))) {
        return std::vector<std::string>{"DEL", match[2].str()};
    }

    if (std::regex_match(normalized, match, std::regex("^(exists)\\s+([^\\s]+)$", std::regex::icase))) {
        return std::vector<std::string>{"EXISTS", match[2].str()};
    }

    if (std::regex_match(normalized, match, std::regex("^does\\s+([^\\s]+)\\s+exist\\??$", std::regex::icase))) {
        return std::vector<std::string>{"EXISTS", match[1].str()};
    }

    if (std::regex_match(normalized, match, std::regex("^(incr|increment|increase)\\s+([^\\s]+)$", std::regex::icase))) {
        return std::vector<std::string>{"INCR", match[2].str()};
    }

    if (std::regex_match(normalized, match, std::regex("^expire\\s+([^\\s]+)\\s+in\\s+([0-9]+)\\s+seconds?$", std::regex::icase))) {
        return std::vector<std::string>{"EXPIRE", match[1].str(), match[2].str()};
    }

    if (std::regex_match(normalized, match, std::regex("^set\\s+ttl\\s+of\\s+([^\\s]+)\\s+to\\s+([0-9]+)\\s+seconds?$", std::regex::icase))) {
        return std::vector<std::string>{"EXPIRE", match[1].str(), match[2].str()};
    }

    if (std::regex_match(normalized, match, std::regex("^(ttl)\\s+([^\\s]+)$", std::regex::icase))) {
        return std::vector<std::string>{"TTL", match[2].str()};
    }

    if (std::regex_match(normalized, match, std::regex("^how\\s+long\\s+until\\s+([^\\s]+)\\s+expires\\??$", std::regex::icase))) {
        return std::vector<std::string>{"TTL", match[1].str()};
    }

    return std::nullopt;
}

}  // namespace makrounadb::common
