#include "endpoint.hpp"

namespace mok::network
{

Result<Endpoint> Endpoint::ipv4(std::string_view ip, uint16_t port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    std::string null_terminated_ip(ip);
    if (::inet_pton(AF_INET, null_terminated_ip.c_str(), &(addr.sin_addr)) == -1)
    {
        return std::unexpected(std::error_code{errno, std::system_category()});
    }

    return Endpoint(addr);
}

Result<Endpoint> Endpoint::ipv6(std::string_view ip, uint16_t port)
{
    sockaddr_in6 addr{};
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);

    std::string null_terminated_ip(ip);
    if (::inet_pton(AF_INET6, null_terminated_ip.c_str(), &addr.sin6_addr) == -1)
    {
        return std::unexpected(std::error_code{errno, std::system_category()});
    }

    return Endpoint(addr);
}

Endpoint Endpoint::ipv4_any(uint16_t port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    return Endpoint(addr);
}

Endpoint Endpoint::ipv6_any(uint16_t port)
{
    sockaddr_in6 addr{};
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    addr.sin6_addr = IN6ADDR_ANY_INIT;
    return Endpoint(addr);
}

Endpoint Endpoint::local_path(std::string_view path)
{
    sockaddr_un addr{};
    addr.sun_family = AF_LOCAL;
    std::strncpy(addr.sun_path, path.data(), sizeof(addr.sun_path) - 1);
    return Endpoint(addr);
}

AddressFamily Endpoint::family() const noexcept
{
    return std::visit([](const auto& addr) -> AddressFamily {
        using T = std::decay_t<decltype(addr)>;
        if constexpr (std::is_same_v<T, sockaddr_in>) return AddressFamily::IPv4;
        else if constexpr (std::is_same_v<T, sockaddr_in6>) return AddressFamily::IPv6;
        else return AddressFamily::Local;
    }, storage);
}

std::string Endpoint::address() const noexcept
{
    std::string addr_str;
    switch (family()) {
        case AddressFamily::IPv4: {
            const auto& addr_in = *reinterpret_cast<const sockaddr_in*>(addr());
            char buf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr_in.sin_addr, buf, sizeof(buf));
            addr_str = buf;
            break;
        }
        case AddressFamily::IPv6: {
            const auto& addr_in6 = *reinterpret_cast<const sockaddr_in6*>(addr());
            char buf[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &addr_in6.sin6_addr, buf, sizeof(buf));
            addr_str = buf;
            break;
        }
        case AddressFamily::Local: {
            const auto* addr_un = reinterpret_cast<const sockaddr_un*>(addr());
            addr_str = addr_un->sun_path;
            break;
        }
    }
    return addr_str;
}

uint16_t Endpoint::port() const noexcept
{
    return std::visit([](const auto& addr) -> uint16_t {
        using T = std::decay_t<decltype(addr)>;
        if constexpr (std::is_same_v<T, sockaddr_in>)  return ntohs(addr.sin_port);
        else if constexpr (std::is_same_v<T, sockaddr_in6>) return ntohs(addr.sin6_port);
        else return 0;
    }, storage);
}

const sockaddr* Endpoint::addr() const noexcept {
    return std::visit([](const auto& addr) -> const sockaddr* {
        return reinterpret_cast<const sockaddr*>(&addr);
    }, storage);
}

socklen_t Endpoint::addrlen() const noexcept {
    return std::visit([](const auto& addr) -> socklen_t {
        return static_cast<socklen_t>(sizeof(addr));
    }, storage);
}

Result<Endpoint> Endpoint::from_storage(const sockaddr_storage& ss, socklen_t len)
{
    switch (ss.ss_family) {
        case AF_INET: {
            if (len < sizeof(sockaddr_in)) return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            const sockaddr_in* addr = reinterpret_cast<const sockaddr_in*>(&ss);
            return Endpoint(*addr);
        }
        case AF_INET6: {
            if (len < sizeof(sockaddr_in6)) return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            const sockaddr_in6* addr = reinterpret_cast<const sockaddr_in6*>(&ss);
            return Endpoint(*addr);
        }
        case AF_LOCAL: {
            if (len < sizeof(sockaddr_un)) return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            const sockaddr_un* addr = reinterpret_cast<const sockaddr_un*>(&ss);
            return Endpoint(*addr);
        }
        default:
            return std::unexpected(std::make_error_code(std::errc::address_family_not_supported));
    }
}

}