#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "socketoptions.hpp"
#include "endpoint.hpp"
#include "result.hpp"

#include <expected>
#include <system_error>
#include <utility>
#include <span>

#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>

namespace mok::network
{

struct AcceptResult;

class Socket 
{
public:
    static Result<Socket> create(AddressFamily family, SocketType type, Protocol protocol = Protocol::Default);
    
    static Result<Socket> tcp_v4();
    static Result<Socket> tcp_v6();
    static Result<Socket> udp_v4();
    static Result<Socket> udp_v6();

    Socket(const Socket&)            = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&&) noexcept;
    Socket& operator=(Socket&&) noexcept;
    
    ~Socket();

    Result<void> bind(const Endpoint& ep) noexcept;
    Result<void> connect(const Endpoint& ep) noexcept;
    Result<void> listen(int backlog = SOMAXCONN) noexcept;

    Result<AcceptResult> accept() const noexcept;

    Result<std::size_t> send(std::span<const std::byte> data, int flags = 0) const noexcept;
    Result<std::size_t> sendto(const Endpoint& ep, std::span<const std::byte> data, int flags = 0) const noexcept;

    Result<std::size_t> receive(std::span<std::byte> buf, int flags = 0) const noexcept;
    Result<std::size_t> recvfrom(Endpoint& ep, std::span<std::byte> buf, int flags = 0) const noexcept;

    Result<void> close() noexcept;
    Result<void> shutdown(int how) noexcept;

    template<typename T>
    Result<void> set_sockopt(SocketOption option, const T& value) noexcept;

    template<typename T>
    Result<T> get_sockopt(SocketOption option) const noexcept;

    Result<void> set_nonblocking(bool enable) noexcept;
    
    int  fd()    const noexcept { return sockfd; }
    bool valid() const noexcept { return sockfd >= 0; }

    /*
    // Query the remote address of an already-connected socket
    Result<Endpoint> peer_endpoint() const;

    // Query the local bound address
    Result<Endpoint> local_endpoint() const;
    */


private:
    int sockfd = -1;
    explicit Socket(int fd) noexcept : sockfd{fd} { }
    SocketType type;
};

template<typename T>
Result<void> Socket::set_sockopt(SocketOption option, const T& value) noexcept {
    const auto [level, optname] = detail::to_native(option);
    if (::setsockopt(sockfd, level, optname, &value, sizeof(T)) < 0)
        return std::unexpected(std::error_code{errno, std::system_category()});
    return {};
}

template<typename T>
Result<T> Socket::get_sockopt(SocketOption option) const noexcept {
    const auto [level, optname] = detail::to_native(option);
    T value{};
    socklen_t len = sizeof(T);
    if (::getsockopt(sockfd, level, optname, &value, &len) < 0)
        return std::unexpected(std::error_code{errno, std::system_category()});
    return value;
}

template<>
inline Result<void> Socket::set_sockopt<bool>(SocketOption option, const bool& value) noexcept {
    return set_sockopt<int>(option, value ? 1 : 0);
}

struct AcceptResult
{
    Socket socket;
    Endpoint endpoint;
};

} // namespace network

#endif // Socket
