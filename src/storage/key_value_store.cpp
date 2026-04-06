#include "storage/key_value_store.hpp"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <vector>

namespace makrounadb::storage {

bool KeyValueStore::is_expired(const std::string& key, std::chrono::steady_clock::time_point now) const {
    const auto expiration_it = expirations_.find(key);
    if (expiration_it == expirations_.end()) {
        return false;
    }
    return now >= expiration_it->second;
}

bool KeyValueStore::erase_if_expired(const std::string& key, std::chrono::steady_clock::time_point now) {
    if (!is_expired(key, now)) {
        return false;
    }

    keyspace_.erase(key);
    expirations_.erase(key);
    return true;
}

void KeyValueStore::set(const std::string& key, const std::string& value) {
    keyspace_[key] = value;
    expirations_.erase(key);
}

std::optional<std::string> KeyValueStore::get(const std::string& key) {
    erase_if_expired(key, std::chrono::steady_clock::now());

    const auto iterator = keyspace_.find(key);
    if (iterator == keyspace_.end()) {
        return std::nullopt;
    }
    return iterator->second;
}

std::size_t KeyValueStore::del(const std::string& key) {
    expirations_.erase(key);
    return keyspace_.erase(key);
}

bool KeyValueStore::exists(const std::string& key) {
    erase_if_expired(key, std::chrono::steady_clock::now());
    return keyspace_.find(key) != keyspace_.end();
}

long long KeyValueStore::incr(const std::string& key) {
    long long current = 0;

    if (const auto existing = get(key); existing.has_value()) {
        std::size_t consumed_characters = 0;
        current = std::stoll(*existing, &consumed_characters);
        if (consumed_characters != existing->size()) {
            throw std::runtime_error("value is not an integer");
        }
    }

    const long long next = current + 1;
    keyspace_[key] = std::to_string(next);
    return next;
}

bool KeyValueStore::expire(const std::string& key, long long seconds) {
    if (!exists(key)) {
        return false;
    }

    if (seconds <= 0) {
        del(key);
        return true;
    }

    expirations_[key] = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    return true;
}

long long KeyValueStore::ttl(const std::string& key) {
    const auto now = std::chrono::steady_clock::now();
    if (erase_if_expired(key, now)) {
        return -2;
    }

    if (keyspace_.find(key) == keyspace_.end()) {
        return -2;
    }

    const auto expiration_it = expirations_.find(key);
    if (expiration_it == expirations_.end()) {
        return -1;
    }

    const auto remaining = std::chrono::duration_cast<std::chrono::seconds>(expiration_it->second - now).count();
    if (remaining < 0) {
        del(key);
        return -2;
    }

    return remaining;
}

std::size_t KeyValueStore::cleanup_expired(std::size_t max_to_remove) {
    if (max_to_remove == 0 || expirations_.empty()) {
        return 0;
    }

    const auto now = std::chrono::steady_clock::now();
    std::size_t removed = 0;
    std::vector<std::string> expired_keys;
    expired_keys.reserve(std::min(max_to_remove, expirations_.size()));

    for (const auto& [key, _] : expirations_) {
        if (removed >= max_to_remove) {
            break;
        }
        if (is_expired(key, now)) {
            expired_keys.push_back(key);
            ++removed;
        }
    }

    for (const auto& key : expired_keys) {
        keyspace_.erase(key);
        expirations_.erase(key);
    }

    return expired_keys.size();
}

}  // namespace makrounadb::storage
