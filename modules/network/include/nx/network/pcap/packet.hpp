#pragma once

#include <nx/common/span.hpp>

#include <cstdint>
#include <cstring>
#include <sys/time.h>

namespace nx::network::pcap {

// ── packet_view ───────────────────────────────────────────────────────────────
//
// Non-owning view of a single captured packet within a packet_batch.
// Valid only while the owning packet_batch has not been cleared or destroyed.

struct packet_view {
    nx::span<const uint8_t> data;  // link-layer frame bytes
    timeval                 ts;    // capture timestamp (sec + usec or nsec)
};

// ── packet_batch ──────────────────────────────────────────────────────────────
//
// Owning batch of captured packets.  Packet data is stored in an inline buffer;
// packet_view entries reference that buffer directly (no heap allocation).
//
// Sized to hold up to `capacity` packets of at most `max_packet_len` bytes each.
// max_packet_len = 9000 covers standard Ethernet (1518 B) and jumbo frames.
// Packets exceeding max_packet_len are skipped by file_reader::read_batch.
//
// Always heap-allocate: sizeof(packet_batch) ≈ 576 KB.

struct packet_batch {
    static constexpr uint32_t capacity       = 64;
    static constexpr uint32_t max_packet_len = 9000;
    static constexpr uint32_t buf_size       = capacity * max_packet_len;

    uint32_t    count    = 0;
    uint32_t    buf_used = 0;
    packet_view packets[capacity];
    uint8_t     buf[buf_size];

    void clear() noexcept { count = 0; buf_used = 0; }

    [[nodiscard]] bool full() const noexcept { return count >= capacity; }

    [[nodiscard]] bool can_fit(uint32_t len) const noexcept {
        return !full() && buf_used + len <= buf_size;
    }
};

} // namespace nx::network::pcap
