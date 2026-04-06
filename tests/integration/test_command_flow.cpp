#include <cassert>
#include <chrono>
#include <thread>

#include "command/command_dispatcher.hpp"
#include "storage/key_value_store.hpp"

int main() {
    makrounadb::storage::KeyValueStore store;
    makrounadb::command::CommandDispatcher dispatcher(store);

    assert(dispatcher.execute({"SET", "k", "v"}) == "+OK\r\n");
    assert(dispatcher.execute({"GET", "k"}) == "$1\r\nv\r\n");
    assert(dispatcher.execute({"INCR", "k"}) == "-ERR value is not an integer or out of range\r\n");
    assert(dispatcher.execute({"INCR", "counter"}) == ":1\r\n");
    assert(dispatcher.execute({"INCR", "counter"}) == ":2\r\n");

    assert(dispatcher.execute({"SET", "session", "alive"}) == "+OK\r\n");
    assert(dispatcher.execute({"EXPIRE", "session", "1"}) == ":1\r\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
    assert(dispatcher.execute({"TTL", "session"}) == ":-2\r\n");
    assert(dispatcher.execute({"GET", "session"}) == "$-1\r\n");

    return 0;
}
