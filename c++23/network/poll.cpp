#include "poll.hpp"

namespace mok::network 
{

Result<std::vector<PollReturnEvent>> poll(std::span<PollEvent> events, std::optional<std::chrono::milliseconds> timeout) noexcept
{
    std::vector<pollfd> poll_fds;
    poll_fds.reserve(events.size());

    for (const auto& event : events) 
    {
        pollfd pfd;
        pfd.fd = event.socket.get().fd();
        pfd.events = 0;
        if (event.receive)     pfd.events |= POLLIN;
        if (event.send)     pfd.events |= POLLOUT;
        if (event.priority) pfd.events |= POLLPRI;
        poll_fds.push_back(pfd);
    }

    int ms = timeout ? static_cast<int>(timeout->count()) : -1;

    int ready = ::poll(poll_fds.data(), poll_fds.size(), ms);
    if (ready < 0)
    {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    std::vector<PollReturnEvent> return_events;
    
    for (std::size_t i = 0; i < events.size(); ++i) {
        const auto& pfd = poll_fds[i];
        if (pfd.revents == 0) continue;

        return_events.push_back({
            .socket   = events[i].socket,
            .receive     = (pfd.revents & POLLIN)  != 0,
            .send     = (pfd.revents & POLLOUT) != 0,
            .priority = (pfd.revents & POLLPRI) != 0,
            .error    = (pfd.revents & POLLERR) != 0,
            .hangup   = (pfd.revents & POLLHUP) != 0,
            .invalid  = (pfd.revents & POLLNVAL) != 0,
        });
    }
    return return_events;
}

}