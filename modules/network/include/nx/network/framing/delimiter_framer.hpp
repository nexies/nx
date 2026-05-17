#pragma once

#include <nx/network/buffer/read_accumulator.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/span.hpp>
#include <nx/core2/object/object.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace nx::network::framing {

// ── delimiter_framer ──────────────────────────────────────────────────────────
//
// Parses a stream framed by a byte sequence (delimiter).
// frame_received fires for each complete frame; delimiter NOT included.
//
// Sending: use the static pack() helper, then write via the socket directly.

class delimiter_framer : public nx::core::object {
public:
    NX_OBJECT(delimiter_framer)
    NX_DISABLE_COPY(delimiter_framer)
    NX_DISABLE_MOVE(delimiter_framer)

    // Single-byte delimiter (e.g. '\n').
    delimiter_framer(read_accumulator * acc, char delim,
                     nx::core::object * parent = nullptr);

    // Multi-byte delimiter (e.g. "\r\n").
    delimiter_framer(read_accumulator * acc, std::string delim,
                     nx::core::object * parent = nullptr);

    // ── Slot ──────────────────────────────────────────────────────────────────

    void on_data();

    // ── Static send helpers ───────────────────────────────────────────────────

    // Returns payload + delimiter appended.
    std::vector<char> pack(const char * data, std::size_t len) const;
    std::vector<char> pack(std::string_view sv)                const;

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(frame_received, nx::span<const char>)

private:
    read_accumulator * acc_;
    std::string        delim_;
};

} // namespace nx::network::framing
