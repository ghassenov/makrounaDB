#include <cassert>
#include <filesystem>

#include "command/command_dispatcher.hpp"
#include "persistence/aof_persistence.hpp"
#include "storage/key_value_store.hpp"

int main() {
    const std::filesystem::path aof_path =
        std::filesystem::temp_directory_path() / "makrounadb_test_appendonly.aof";
    std::filesystem::remove(aof_path);

    {
        makrounadb::storage::KeyValueStore store;
        makrounadb::command::CommandDispatcher dispatcher(store);
        makrounadb::persistence::AofPersistence persistence(aof_path.string());

        assert(persistence.open());

        const auto set_response = dispatcher.execute({"SET", "k", "v"});
        persistence.append_if_write({"SET", "k", "v"}, set_response);

        const auto incr_response = dispatcher.execute({"INCR", "counter"});
        persistence.append_if_write({"INCR", "counter"}, incr_response);

        const auto get_response = dispatcher.execute({"GET", "k"});
        persistence.append_if_write({"GET", "k"}, get_response);

        persistence.flush();
    }

    {
        makrounadb::storage::KeyValueStore restored_store;
        makrounadb::command::CommandDispatcher restored_dispatcher(restored_store);
        makrounadb::persistence::AofPersistence restored_persistence(aof_path.string());

        assert(restored_persistence.load_and_replay(restored_dispatcher));
        assert(restored_dispatcher.execute({"GET", "k"}) == "$1\r\nv\r\n");
        assert(restored_dispatcher.execute({"GET", "counter"}) == "$1\r\n1\r\n");
    }

    std::filesystem::remove(aof_path);
    return 0;
}
