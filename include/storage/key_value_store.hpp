#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <unordered_map>

namespace makrounadb::storage {

class KeyValueStore {
public:
    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    std::size_t del(const std::string& key);
    bool exists(const std::string& key);
    long long incr(const std::string& key);
    long long incrby(const std::string& key, long long amount);
    long long decr(const std::string& key);
    long long decrby(const std::string& key, long long amount);
    long long strlen(const std::string& key);
    bool setnx(const std::string& key, const std::string& value);
    bool persist(const std::string& key);
    std::optional<std::string> getdel(const std::string& key);
    bool rename(const std::string& from, const std::string& to);
    bool expire(const std::string& key, long long seconds);
    long long ttl(const std::string& key);
    std::size_t cleanup_expired(std::size_t max_to_remove = 128);

private:
    bool is_expired(const std::string& key, std::chrono::steady_clock::time_point now) const;
    bool erase_if_expired(const std::string& key, std::chrono::steady_clock::time_point now);

    std::unordered_map<std::string, std::string> keyspace_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> expirations_;
};

}  // namespace makrounadb::storage
