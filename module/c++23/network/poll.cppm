module;

#include <vector>
#include <expected>
#include <optional>
#include <functional>
#include <span>
#include <chrono>
#include <poll.h>

export module network.poll;
import network.result;
import network.socket;

export namespace network 
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
