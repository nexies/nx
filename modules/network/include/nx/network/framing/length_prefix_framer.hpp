#pragma once

#include <nx/network/buffer/read_accumulator.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/span.hpp>
#include <nx/core2/object/object.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace nx::network::framing {

// ── length_prefix_framer ──────────────────────────────────────────────────────
//
// Parses a stream framed with a 4-byte big-endian length prefix.
// Wire format:  [ uint32_t length (BE) ][ payload bytes ]
//
// Receiving:
//   Connect acc->data_available to on_data(), or pass acc to the constructor
//   (auto-connects internally).  frame_received fires when a complete frame arrives.
//
// Sending:
//   Use the static pack() helper to build a framed buffer, then write it via
//   the socket directly.

class length_prefix_framer : public nx::core::object {
public:
    NX_OBJECT(length_prefix_framer)
    NX_DISABLE_COPY(length_prefix_framer)
    NX_DISABLE_MOVE(length_prefix_framer)

    // Connects to acc->data_available automatically.
    explicit length_prefix_framer(read_accumulator * acc,
                                   nx::core::object * parent = nullptr);

    // ── Slot ──────────────────────────────────────────────────────────────────

    void on_data();

    // ── Static send helpers ───────────────────────────────────────────────────

    // Returns a framed buffer: [4-byte BE length][payload].
    static std::vector<char> pack(const char * data, std::size_t len);
    static std::vector<char> pack(nx::span<const char> data);

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(frame_received, nx::span<const char>)

private:
    read_accumulator * acc_;
    std::uint32_t      pending_len_ { 0 };
    bool               has_header_  { false };

    static constexpr std::size_t header_size = 4;
};

} // namespace nx::network::framing
