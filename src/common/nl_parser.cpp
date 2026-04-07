#include "common/nl_parser.hpp"

#include <algorithm>
#include <cctype>
#include <regex>

namespace makrounadb::common {
namespace {

std::string trim(const std::string &value) {
    std::size_t start = 0;
    while (start < value.size() &&
           std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    std::size_t end = value.size();
    while (end > start &&
           std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}

std::string to_lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    return value;
}

} // namespace

std::optional<std::vector<std::string>>
parse_natural_language_command(const std::string &input) {
    const std::string normalized = trim(input);
    if (normalized.empty()) {
        return std::nullopt;
    }

    const std::string lower = to_lower(normalized);
    std::smatch match;

    if (std::regex_match(lower, std::regex("^(ping|are you alive|hello)$"))) {
        return std::vector<std::string>{"PING"};
    }

    if (std::regex_match(normalized, match,
                         std::regex("^echo\\s+(.+)$", std::regex::icase))) {
        return std::vector<std::string>{"ECHO", trim(match[1].str())};
    }

    if (std::regex_match(
            normalized, match,
            std::regex("^(set|remember)\\s+([^\\s]+)\\s+(to|as)\\s+(.+)$",
                       std::regex::icase))) {
        return std::vector<std::string>{"SET", match[2].str(),
                                        trim(match[4].str())};
    }

    if (std::regex_match(
            normalized, match,
            std::regex("^(get|show)\\s+([^\\s]+)$", std::regex::icase))) {
        return std::vector<std::string>{"GET", match[2].str()};
    }

    if (std::regex_match(normalized, match,
                         std::regex("^what\\s+is\\s+([^\\s\\?]+)\\??$",
                                    std::regex::icase))) {
        return std::vector<std::string>{"GET", match[1].str()};
    }

    if (std::regex_match(
            normalized, match,
            std::regex("^(delete|remove)\\s+([^\\s]+)$", std::regex::icase))) {
        return std::vector<std::string>{"DEL", match[2].str()};
    }

    if (std::regex_match(
            normalized, match,
            std::regex("^(exists)\\s+([^\\s]+)$", std::regex::icase))) {
        return std::vector<std::string>{"EXISTS", match[2].str()};
    }

    if (std::regex_match(normalized, match,
                         std::regex("^does\\s+([^\\s]+)\\s+exist\\??$",
                                    std::regex::icase))) {
        return std::vector<std::string>{"EXISTS", match[1].str()};
    }

    if (std::regex_match(normalized, match,
                         std::regex("^(incr|increment|increase)\\s+([^\\s]+)$",
                                    std::regex::icase))) {
        return std::vector<std::string>{"INCR", match[2].str()};
    }

    if (std::regex_match(
            normalized, match,
            std::regex("^expire\\s+([^\\s]+)\\s+in\\s+([0-9]+)\\s+seconds?$",
                       std::regex::icase))) {
        return std::vector<std::string>{"EXPIRE", match[1].str(),
                                        match[2].str()};
    }

    if (std::regex_match(
            normalized, match,
            std::regex(
                "^set\\s+ttl\\s+of\\s+([^\\s]+)\\s+to\\s+([0-9]+)\\s+seconds?$",
                std::regex::icase))) {
        return std::vector<std::string>{"EXPIRE", match[1].str(),
                                        match[2].str()};
    }

    if (std::regex_match(
            normalized, match,
            std::regex("^(ttl)\\s+([^\\s]+)$", std::regex::icase))) {
        return std::vector<std::string>{"TTL", match[2].str()};
    }

    if (std::regex_match(
            normalized, match,
            std::regex("^how\\s+long\\s+until\\s+([^\\s]+)\\s+expires\\??$",
                       std::regex::icase))) {
        return std::vector<std::string>{"TTL", match[1].str()};
    }

    // RIGATONI: decrement by 1
    if (std::regex_match(
            normalized, match,
            std::regex("^(decr|decrement|decrease)\\s+([^\\s]+)$",
                       std::regex::icase))) {
        return std::vector<std::string>{"RIGATONI", match[2].str()};
    }

    // LINGUINE: increment by N
    if (std::regex_match(
            normalized, match,
            std::regex("^(increment|increase|add)\\s+([^\\s]+)\\s+by\\s+([0-9]+)$",
                       std::regex::icase))) {
        return std::vector<std::string>{"LINGUINE", match[2].str(), match[3].str()};
    }

    // VERMICELLI: decrement by N
    if (std::regex_match(
            normalized, match,
            std::regex("^(decrement|decrease|subtract)\\s+([^\\s]+)\\s+by\\s+([0-9]+)$",
                       std::regex::icase))) {
        return std::vector<std::string>{"VERMICELLI", match[2].str(), match[3].str()};
    }

    // SPAGHETTI: string length
    if (std::regex_match(
            normalized, match,
            std::regex("^(length|strlen)\\s+of\\s+([^\\s]+)$",
                       std::regex::icase))) {
        return std::vector<std::string>{"SPAGHETTI", match[2].str()};
    }

    if (std::regex_match(
            normalized, match,
            std::regex("^how\\s+long\\s+is\\s+([^\\s\\?]+)\\??$",
                       std::regex::icase))) {
        return std::vector<std::string>{"SPAGHETTI", match[1].str()};
    }

    // PENNE: set only if key is absent
    if (std::regex_match(
            normalized, match,
            std::regex("^set\\s+([^\\s]+)\\s+(to|as)\\s+(.+)\\s+if\\s+(not\\s+exists?|missing|absent)$",
                       std::regex::icase))) {
        return std::vector<std::string>{"PENNE", match[1].str(), trim(match[3].str())};
    }

    // ALDENTE: remove expiry from key
    if (std::regex_match(
            normalized, match,
            std::regex("^(persist|make\\s+permanent)\\s+([^\\s]+)$",
                       std::regex::icase))) {
        return std::vector<std::string>{"ALDENTE", match[2].str()};
    }

    if (std::regex_match(
            normalized, match,
            std::regex("^remove\\s+(expiry|expiration|ttl)\\s+(of|from)\\s+([^\\s]+)$",
                       std::regex::icase))) {
        return std::vector<std::string>{"ALDENTE", match[3].str()};
    }

    // FARFALLE: get and delete
    if (std::regex_match(
            normalized, match,
            std::regex("^(get\\s+and\\s+delete|fetch\\s+and\\s+remove|getdel)\\s+([^\\s]+)$",
                       std::regex::icase))) {
        return std::vector<std::string>{"FARFALLE", match[2].str()};
    }

    // LASAGNA: rename a key
    if (std::regex_match(
            normalized, match,
            std::regex("^rename\\s+([^\\s]+)\\s+to\\s+([^\\s]+)$",
                       std::regex::icase))) {
        return std::vector<std::string>{"LASAGNA", match[1].str(), match[2].str()};
    }

    return std::nullopt;
}

} // namespace makrounadb::common
