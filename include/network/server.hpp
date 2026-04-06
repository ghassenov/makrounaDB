#pragma once

#include <atomic>
#include <cstdint>

#include "command/command_dispatcher.hpp"
#include "persistence/aof_persistence.hpp"

namespace makrounadb::network {

class Server {
public:
    explicit Server(
        command::CommandDispatcher& dispatcher,
        std::uint16_t port = 6379,
        persistence::AofPersistence* persistence = nullptr
    );
    void run();
    void stop();
    bool is_running() const;

private:
    bool setup_listener();
    void event_loop();

    command::CommandDispatcher& dispatcher_;
    persistence::AofPersistence* persistence_;
    std::uint16_t port_;
    std::atomic<bool> running_{false};
    int listener_fd_ = -1;
};

}  // namespace makrounadb::network
