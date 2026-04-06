#pragma once

#include <string>
#include <vector>

#include "storage/key_value_store.hpp"

namespace makrounadb::command {

class CommandDispatcher {
public:
    explicit CommandDispatcher(storage::KeyValueStore& store);
    std::string execute(const std::vector<std::string>& arguments);
    std::size_t run_maintenance();

private:
    storage::KeyValueStore& store_;
};

}  // namespace makrounadb::command
