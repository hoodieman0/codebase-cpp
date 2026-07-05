module;
#include <expected>
#include <system_error>
#include <utility>
#include <span>

#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>

module network.socket;

namespace network {

static std::error_code current_error() 
{
    return {errno, std::system_category()};
}

std::expected<Socket, std::error_code> Socket::create(
    AddressFamily family, SocketType type, Protocol protocol)
{
    int _domain = detail::to_native(family);
    int _type = detail::to_native(type);
    int _protocol = detail::to_native(protocol);
    int fd = ::socket(_domain, _type, _protocol);
    if (fd < 0) return std::unexpected(current_error());
    return Socket{fd};
}

Result<Socket> Socket::tcp_v4()
{
    return create(AddressFamily::IPv4, SocketType::Stream, Protocol::TCP);
}

Result<Socket> Socket::tcp_v6()
{
    return create(AddressFamily::IPv6, SocketType::Stream, Protocol::TCP);
}

Result<Socket> Socket::udp_v4()
{
    return create(AddressFamily::IPv4, SocketType::Datagram, Protocol::UDP);
}

Result<Socket> Socket::udp_v6()
{
    return create(AddressFamily::IPv6, SocketType::Datagram, Protocol::UDP);
}

Socket::Socket(Socket&& other) noexcept
    : sockfd{other.sockfd}, type{other.type}
{
    other.sockfd = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept
{
    if (this != &other)
    {
        this->close();
        sockfd = other.sockfd;
        type = other.type;
        other.sockfd = -1;
    }
    return *this;
}

Socket::~Socket()
{
    this->close();
}

Result<void> Socket::bind(const Endpoint& ep) noexcept
{
    if (sockfd == -1) return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    if (::bind(sockfd, ep.addr(), ep.addrlen()) == -1)
        return std::unexpected(current_error());
    return {};
}

Result<void> Socket::connect(const Endpoint& ep) noexcept
{
    if (sockfd == -1) return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    if (::connect(sockfd, ep.addr(), ep.addrlen()) == -1)
        return std::unexpected(current_error());
    return {};
}

Result<void> Socket::listen(int backlog) noexcept
{
    if (sockfd == -1) return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    if (::listen(sockfd, backlog) == -1)
        return std::unexpected(current_error());
    return {};
}

Result<AcceptResult> Socket::accept() const noexcept
{
    if (sockfd == -1) return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));

    sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);
    int fd = ::accept(sockfd, reinterpret_cast<sockaddr*>(&addr), &addrlen);
    if (fd == -1) return std::unexpected(current_error());

    auto ep = Endpoint::from_storage(addr, addrlen);
    if (!ep)
    {
        ::close(fd);
        return std::unexpected(ep.error());
    }

    return AcceptResult{Socket{fd}, ep.value()};
}

Result<std::size_t> Socket::send(std::span<const std::byte> data, int flags) const noexcept
{
    if (sockfd == -1) return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    ssize_t sent = ::send(sockfd, data.data(), data.size(), flags);
    if (sent == -1) return std::unexpected(current_error());
    return sent;
}

Result<std::size_t> Socket::sendto(const Endpoint& ep, std::span<const std::byte> data, int flags) const noexcept
{
    if (sockfd == -1) return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    ssize_t sent = ::sendto(sockfd, data.data(), data.size(), flags, ep.addr(), ep.addrlen());
    if (sent == -1) return std::unexpected(current_error());
    return sent;
}

Result<std::size_t> Socket::receive(std::span<std::byte> buf, int flags) const noexcept
{
    if (sockfd == -1) return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    ssize_t received = ::recv(sockfd, buf.data(), buf.size(), flags);
    if (received == -1) return std::unexpected(current_error());
    return received;
}

Result<std::size_t> Socket::recvfrom(Endpoint& ep, std::span<std::byte> buf, int flags) const noexcept
{
    if (sockfd == -1) return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));

    sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);
    ssize_t received = ::recvfrom(sockfd, buf.data(), buf.size(), flags, reinterpret_cast<sockaddr*>(&addr), &addrlen);
    if (received == -1) return std::unexpected(current_error());

    auto endpoint = Endpoint::from_storage(addr, addrlen);
    if (!endpoint) return std::unexpected(endpoint.error());
    ep = endpoint.value();

    return received;
}

Result<void> Socket::close() noexcept
{
    if (sockfd == -1) return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    if (::close(sockfd) == -1) return std::unexpected(current_error());
    sockfd = -1;
    return {};
}

Result<void> Socket::shutdown(int how) noexcept
{
    if (sockfd == -1) return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    if (::shutdown(sockfd, how) == -1) return std::unexpected(current_error());
    return {};
}

Result<void> Socket::set_nonblocking(bool enabled) noexcept
{
    if (sockfd == -1) return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) return std::unexpected(current_error());
    if (enabled)    flags |= O_NONBLOCK;
    else            flags &= ~O_NONBLOCK;
    if (::fcntl(sockfd, F_SETFL, flags) == -1) return std::unexpected(current_error());
    return {};
}

} // namespace network
