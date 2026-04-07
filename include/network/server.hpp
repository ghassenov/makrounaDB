#pragma once

#include <atomic>
#include <cstdint>

#ifdef _WIN32
#include <winsock2.h>
#endif

#include "command/command_dispatcher.hpp"
#include "persistence/aof_persistence.hpp"

namespace makrounadb::network {

#ifdef _WIN32
using SocketHandle = SOCKET;
#else
using SocketHandle = int;
#endif

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
    SocketHandle listener_fd_ =
#ifdef _WIN32
        INVALID_SOCKET;
#else
        -1;
#endif
};

}  // namespace makrounadb::network
