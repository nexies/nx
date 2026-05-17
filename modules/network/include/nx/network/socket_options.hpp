#pragma once

#include <cstdint>

namespace nx::network {

// ── opt_level / opt_name ──────────────────────────────────────────────────────
//
// Platform-independent identifiers mapped to actual setsockopt constants inside
// the platform socket_impl.  New entries require matching cases in the impl.

enum class opt_level : uint8_t {
    socket,   // SOL_SOCKET
    tcp,      // IPPROTO_TCP
    ip,       // IPPROTO_IP
    ipv6,     // IPPROTO_IPV6
};

enum class opt_name : uint8_t {
    reuse_address,  // SO_REUSEADDR
    no_delay,       // TCP_NODELAY
    recv_buf_size,  // SO_RCVBUF
    send_buf_size,  // SO_SNDBUF
    broadcast,      // SO_BROADCAST
    ip_ttl,         // IP_TTL
    ipv6_only,      // IPV6_V6ONLY
};

// ── basic_option ──────────────────────────────────────────────────────────────
//
// Tag type that pairs a level+name with a C++ value type.
// All pre-defined aliases live in nx::network::opt.
//
// Usage:
//   socket.set_option(opt::no_delay { true });
//   auto r = socket.get_option<opt::recv_buf_size>();  // result<int>

template<opt_level Level, opt_name Name, typename T>
struct basic_option {
    using value_type = T;

    static constexpr opt_level level = Level;
    static constexpr opt_name  name  = Name;

    T value {};

    basic_option() = default;
    explicit basic_option(T v) : value(v) {}
};

// ── Pre-defined options ───────────────────────────────────────────────────────

namespace opt {

using reuse_address = basic_option<opt_level::socket, opt_name::reuse_address, bool>;
using no_delay      = basic_option<opt_level::tcp,    opt_name::no_delay,      bool>;
using recv_buf_size = basic_option<opt_level::socket, opt_name::recv_buf_size, int>;
using send_buf_size = basic_option<opt_level::socket, opt_name::send_buf_size, int>;
using broadcast     = basic_option<opt_level::socket, opt_name::broadcast,     bool>;
using ip_ttl        = basic_option<opt_level::ip,     opt_name::ip_ttl,        int>;
using ipv6_only     = basic_option<opt_level::ipv6,   opt_name::ipv6_only,     bool>;

} // namespace opt

} // namespace nx::network
