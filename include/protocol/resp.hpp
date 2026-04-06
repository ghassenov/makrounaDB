#pragma once

#include <optional>
#include <string>

namespace makrounadb::protocol {

std::string simple_string(const std::string& value);
std::string bulk_string(const std::string& value);
std::string null_bulk_string();
std::string integer(long long value);
std::string error(const std::string& message);

}  // namespace makrounadb::protocol
