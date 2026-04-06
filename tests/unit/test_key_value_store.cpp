#include <cassert>
#include <chrono>
#include <thread>

#include "storage/key_value_store.hpp"

int main() {
    makrounadb::storage::KeyValueStore store;

    store.set("name", "makrouna");
    assert(store.get("name").has_value());
    assert(store.get("name").value() == "makrouna");

    assert(store.exists("name"));
    assert(store.del("name") == 1);
    assert(!store.exists("name"));

    assert(store.incr("counter") == 1);
    assert(store.incr("counter") == 2);

    store.set("ttl_key", "value");
    assert(store.ttl("ttl_key") == -1);
    assert(store.expire("ttl_key", 1));
    const long long ttl_now = store.ttl("ttl_key");
    assert(ttl_now == 1 || ttl_now == 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
    assert(store.ttl("ttl_key") == -2);
    assert(!store.get("ttl_key").has_value());

    store.set("cleanup_key", "value");
    assert(store.expire("cleanup_key", 1));
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
    assert(store.cleanup_expired() >= 1);
    assert(!store.exists("cleanup_key"));

    return 0;
}
