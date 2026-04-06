#pragma once

#include <optional>
#include <string>
#include <vector>

namespace makrounadb::common {

std::optional<std::vector<std::string>> parse_natural_language_command(const std::string& input);

}  // namespace makrounadb::common
