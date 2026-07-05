#ifndef NETWORK_ENDPOINT_HPP
#define NETWORK_ENDPOINT_HPP

#include <system_error>
#include <expected>
#include <variant>
#include <string_view>
#include <cstring>
#include <cstdint>
#include <utility>
#include <format>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>

#include "result.hpp"
#include "socketoptions.hpp"

namespace mok::network 
{

class Endpoint 
{
public:
    static Result<Endpoint> ipv4(std::string_view ip, uint16_t port);
    static Result<Endpoint> ipv6(std::string_view ip, uint16_t port);
    static Endpoint ipv4_any(uint16_t port);
    static Endpoint ipv6_any(uint16_t port);
    static Endpoint local_path(std::string_view path);
    static Result<Endpoint> from_storage(const sockaddr_storage& ss, socklen_t len);

    AddressFamily family() const noexcept;
    std::string address() const noexcept;
    uint16_t port() const noexcept;

    const sockaddr* addr() const noexcept;
    socklen_t addrlen() const noexcept;

private:
    using Storage = std::variant<sockaddr_in, sockaddr_in6, sockaddr_un>;

    explicit Endpoint(Storage storage) : storage(std::move(storage)) {}
    Storage storage;
};

}

template<>
struct std::formatter<mok::network::Endpoint> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const mok::network::Endpoint& ep, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "{}:{}", ep.address(), ep.port());
    }
};

#endif // NETWORK_ENDPOINT_HPP
