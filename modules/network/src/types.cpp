#include <nx/network/types.hpp>
#include <nx/common/types/errors/codes.hpp>

#include <algorithm>
#include <cstring>

#if defined(NX_OS_WINDOWS)
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#endif

namespace nx::network {

// ── ip_address — named constructors ──────────────────────────────────────────

ip_address ip_address::any_v4() noexcept
{
    ip_address a;
    a.family_ = socket_family::ipv4;
    a.bytes_.fill(0);
    return a;
}

ip_address ip_address::any_v6() noexcept
{
    ip_address a;
    a.family_ = socket_family::ipv6;
    a.bytes_.fill(0);
    return a;
}

ip_address ip_address::loopback_v4() noexcept
{
    ip_address a;
    a.family_    = socket_family::ipv4;
    a.bytes_[0]  = 127;
    a.bytes_[1]  = 0;
    a.bytes_[2]  = 0;
    a.bytes_[3]  = 1;
    return a;
}

ip_address ip_address::loopback_v6() noexcept
{
    ip_address a;
    a.family_ = socket_family::ipv6;
    a.bytes_.fill(0);
    a.bytes_[15] = 1;
    return a;
}

ip_address ip_address::from_bytes(socket_family family,
                                   const uint8_t * bytes,
                                   std::size_t len) noexcept
{
    ip_address a;
    a.family_ = family;
    const std::size_t n = std::min(len, a.bytes_.size());
    std::memcpy(a.bytes_.data(), bytes, n);
    return a;
}

// ── ip_address::from_string ───────────────────────────────────────────────────

nx::result<ip_address> ip_address::from_string(std::string_view s) noexcept
{
    const std::string str(s);

    // Try IPv4 first
    ip_address a4;
    a4.family_ = socket_family::ipv4;
    if (::inet_pton(AF_INET, str.c_str(), a4.bytes_.data()) == 1)
        return a4;

    // Try IPv6
    ip_address a6;
    a6.family_ = socket_family::ipv6;
    if (::inet_pton(AF_INET6, str.c_str(), a6.bytes_.data()) == 1)
        return a6;

    return nx::err::invalid_argument("ip_address::from_string: invalid address");
}

// ── ip_address::to_string ─────────────────────────────────────────────────────

std::string ip_address::to_string() const
{
    char buf[INET6_ADDRSTRLEN] {};
    const int af = is_v4() ? AF_INET : AF_INET6;
    if (::inet_ntop(af, bytes_.data(), buf, sizeof(buf)))
        return buf;
    return "(invalid)";
}

bool ip_address::operator==(const ip_address & o) const noexcept {
    return family_ == o.family_
        && memcmp(bytes_.data(), o.bytes_.data(), byte_count()) == 0;
}

// ── ip_address predicates ─────────────────────────────────────────────────────

bool ip_address::is_loopback() const noexcept
{
    if (is_v4())
        return bytes_[0] == 127;
    // IPv6 loopback: ::1
    for (int i = 0; i < 15; ++i)
        if (bytes_[i] != 0) return false;
    return bytes_[15] == 1;
}

bool ip_address::is_any() const noexcept
{
    for (auto b : bytes_)
        if (b != 0) return false;
    return true;
}

// ── endpoint::to_string ───────────────────────────────────────────────────────

std::string endpoint::to_string() const
{
    if (address.is_v6())
        return '[' + address.to_string() + "]:" + std::to_string(port);
    return address.to_string() + ':' + std::to_string(port);
}

bool endpoint::operator==(const endpoint & o) const noexcept {
    return address == o.address && port == o.port;
}
} // namespace nx::network
