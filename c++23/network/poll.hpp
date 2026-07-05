#ifndef NETWORK_POLL_HPP
#define NETWORK_POLL_HPP

#include <vector>
#include <expected>
#include <optional>
#include <functional>
#include <span>
#include <chrono>
#include <poll.h>

#include "result.hpp"
#include "socket.hpp"

namespace mok::network 
{

struct PollEvent
{
    std::reference_wrapper<Socket> socket;
    bool receive = false;
    bool send = false;
    bool priority = false;
};

struct PollReturnEvent
{
    std::reference_wrapper<Socket> socket;
    bool receive = false;
    bool send = false;
    bool priority = false;
    bool error = false;
    bool hangup = false;
    bool invalid = false;
};

Result<std::vector<PollReturnEvent>> poll(std::span<PollEvent> events, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;

}

#endif // NETWORK_POLL_HPP