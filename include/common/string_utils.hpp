#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace makrounadb::common {

inline std::string to_upper(std::string value) {
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char character) { return static_cast<char>(std::toupper(character)); }
    );
    return value;
}

}  // namespace makrounadb::common
