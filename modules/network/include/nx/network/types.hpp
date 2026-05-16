#pragma once

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace nx::network {

// ── socket_family / socket_type ───────────────────────────────────────────────

enum class socket_family : uint8_t { ipv4, ipv6 };
enum class socket_type   : uint8_t { tcp, udp, raw };

// ── ip_address ────────────────────────────────────────────────────────────────

class ip_address {
public:
    // Named constructors
    NX_NODISCARD static ip_address any_v4()      noexcept;
    NX_NODISCARD static ip_address any_v6()      noexcept;
    NX_NODISCARD static ip_address loopback_v4() noexcept;
    NX_NODISCARD static ip_address loopback_v6() noexcept;

    NX_NODISCARD static nx::result<ip_address> from_string(std::string_view s) noexcept;

    // From raw network-order bytes (IPv4: 4 bytes, IPv6: 16 bytes)
    NX_NODISCARD static ip_address from_bytes(socket_family family,
                                               const uint8_t * bytes,
                                               std::size_t len) noexcept;

    // Properties
    NX_NODISCARD socket_family family()     const noexcept { return family_; }
    NX_NODISCARD bool          is_v4()      const noexcept { return family_ == socket_family::ipv4; }
    NX_NODISCARD bool          is_v6()      const noexcept { return family_ == socket_family::ipv6; }
    NX_NODISCARD bool          is_loopback() const noexcept;
    NX_NODISCARD bool          is_any()      const noexcept;

    NX_NODISCARD std::string to_string() const;

    // Raw network-order byte access (impl layer only)
    NX_NODISCARD const uint8_t * bytes()      const noexcept { return bytes_.data(); }
    NX_NODISCARD std::size_t     byte_count() const noexcept { return is_v4() ? 4u : 16u; }

    bool operator==(const ip_address &) const noexcept;;

private:
    socket_family            family_ { socket_family::ipv4 };
    std::array<uint8_t, 16> bytes_  {};
};

// ── endpoint ──────────────────────────────────────────────────────────────────

struct endpoint {
    ip_address address;
    uint16_t   port { 0 };

    endpoint() = default;
    endpoint(ip_address addr, uint16_t p) : address(std::move(addr)), port(p) {}

    NX_NODISCARD std::string to_string() const;

    bool operator==(const endpoint &) const noexcept;
};

} // namespace nx::network
