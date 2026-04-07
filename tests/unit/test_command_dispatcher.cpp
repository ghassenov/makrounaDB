#include <cassert>
#include <chrono>
#include <thread>
#include <vector>

#include "command/command_dispatcher.hpp"
#include "storage/key_value_store.hpp"

int main() {
    makrounadb::storage::KeyValueStore store;
    makrounadb::command::CommandDispatcher dispatcher(store);

    assert(dispatcher.execute({"PING"}) == "+PONG\r\n");
    assert(dispatcher.execute({"ECHO", "hello"}) == "$5\r\nhello\r\n");
    assert(dispatcher.execute({"SET", "x", "42"}) == "+OK\r\n");
    assert(dispatcher.execute({"GET", "x"}) == "$2\r\n42\r\n");
    assert(dispatcher.execute({"INCR", "x"}) == ":43\r\n");
    assert(dispatcher.execute({"EXISTS", "x", "y"}) == ":1\r\n");
    assert(dispatcher.execute({"DEL", "x", "missing"}) == ":1\r\n");
    assert(dispatcher.execute({"GET", "x"}) == "$-1\r\n");

    assert(dispatcher.execute({"SET", "tmp", "1"}) == "+OK\r\n");
    assert(dispatcher.execute({"EXPIRE", "tmp", "1"}) == ":1\r\n");
    const std::string ttl_before_expiration = dispatcher.execute({"TTL", "tmp"});
    assert(ttl_before_expiration == ":1\r\n" || ttl_before_expiration == ":0\r\n");

    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
    assert(dispatcher.execute({"TTL", "tmp"}) == ":-2\r\n");
    assert(dispatcher.execute({"GET", "tmp"}) == "$-1\r\n");

    assert(dispatcher.execute({"SET", "ephemeral", "v"}) == "+OK\r\n");
    assert(dispatcher.execute({"EXPIRE", "ephemeral", "0"}) == ":1\r\n");
    assert(dispatcher.execute({"EXISTS", "ephemeral"}) == ":0\r\n");

    return 0;
}
