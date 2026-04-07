#pragma once

#include <chrono>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace makrounadb::command {
class CommandDispatcher;
}

namespace makrounadb::persistence {

class AofPersistence {
public:
    explicit AofPersistence(
        std::string file_path,
        std::chrono::seconds fsync_interval = std::chrono::seconds(1)
    );

    bool open();
    bool load_and_replay(command::CommandDispatcher& dispatcher);
    void append_if_write(const std::vector<std::string>& arguments, const std::string& response);
    void periodic_sync();
    void flush();

private:
    static bool is_write_command(const std::vector<std::string>& arguments);
    static std::string encode_as_resp_array(const std::vector<std::string>& arguments);

    std::string file_path_;
    std::chrono::seconds fsync_interval_;
    std::chrono::steady_clock::time_point next_sync_;
    std::ofstream output_;
};

}  // namespace makrounadb::persistence
