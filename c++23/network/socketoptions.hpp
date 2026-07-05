#ifndef NETWORK_SOCKETOPTIONS_HPP
#define NETWORK_SOCKETOPTIONS_HPP

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <utility>

namespace mok::network
{
    enum class AddressFamily { IPv4, IPv6, Local };
    enum class SocketType { Stream, Datagram, SequencePacket };
    enum class Protocol { Default, TCP, UDP, SCTP, UDPLite, Raw };

    // TODO update this enum to include more socket options and their corresponding levels
    enum class SocketOption {
        // SOL_SOCKET level
        ReuseAddress,   // SO_REUSEADDR
        ReusePort,      // SO_REUSEPORT
        KeepAlive,      // SO_KEEPALIVE
        SendBuffer,     // SO_SNDBUF
        ReceiveBuffer,  // SO_RCVBUF
        Linger,         // SO_LINGER
        Error,          // SO_ERROR
        Broadcast,      // SO_BROADCAST

        // IPPROTO_TCP level
        NoDelay,        // TCP_NODELAY
        KeepIdle,       // TCP_KEEPIDLE
        KeepInterval,   // TCP_KEEPINTVL
        KeepCount,      // TCP_KEEPCNT
    };


namespace detail 
{
    constexpr int to_native(AddressFamily af)
    {
        switch (af)
        {
            case AddressFamily::IPv4:   return AF_INET;
            case AddressFamily::IPv6:   return AF_INET6;
            case AddressFamily::Local:  return AF_LOCAL; 
        }
        std::unreachable();
    }

    constexpr int to_native(SocketType st)
    {
        switch (st)
        {
            case SocketType::Stream:            return SOCK_STREAM;
            case SocketType::Datagram:          return SOCK_DGRAM;
            case SocketType::SequencePacket:    return SOCK_SEQPACKET;
        }
        std::unreachable();
    }

    constexpr int to_native(Protocol p)
    {
        switch (p)
        {
            case Protocol::Default: return 0;
            case Protocol::TCP:     return IPPROTO_TCP;
            case Protocol::UDP:     return IPPROTO_UDP;
            case Protocol::SCTP:    return IPPROTO_SCTP;
            case Protocol::UDPLite: return IPPROTO_UDPLITE;
            case Protocol::Raw:     return IPPROTO_RAW;
        }
        std::unreachable();
    }

    struct NativeOption {
        int level;
        int optname;
    };

    constexpr NativeOption to_native(SocketOption opt) {
        switch (opt) {
            case SocketOption::ReuseAddress:  return {SOL_SOCKET,  SO_REUSEADDR};
            case SocketOption::ReusePort:     return {SOL_SOCKET,  SO_REUSEPORT};
            case SocketOption::KeepAlive:     return {SOL_SOCKET,  SO_KEEPALIVE};
            case SocketOption::SendBuffer:    return {SOL_SOCKET,  SO_SNDBUF};
            case SocketOption::ReceiveBuffer: return {SOL_SOCKET,  SO_RCVBUF};
            case SocketOption::Linger:        return {SOL_SOCKET,  SO_LINGER};
            case SocketOption::Error:         return {SOL_SOCKET,  SO_ERROR};
            case SocketOption::Broadcast:     return {SOL_SOCKET,  SO_BROADCAST};
            case SocketOption::NoDelay:       return {IPPROTO_TCP, TCP_NODELAY};
            case SocketOption::KeepIdle:      return {IPPROTO_TCP, TCP_KEEPIDLE};
            case SocketOption::KeepInterval:  return {IPPROTO_TCP, TCP_KEEPINTVL};
            case SocketOption::KeepCount:     return {IPPROTO_TCP, TCP_KEEPCNT};
        }
        std::unreachable();
    }
}

} // namespace network

#endif