#pragma once

#include <nx/network/buffer/read_buffer.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/span.hpp>
#include <nx/core2/object/object.hpp>

#include <cstddef>
#include <optional>
#include <string_view>

namespace nx::network {

// ── read_accumulator ──────────────────────────────────────────────────────────
//
// Accumulates incoming byte chunks into a read_buffer.
//
// Feed data via the feed() slot (connect to tcp::socket::data_received or any
// other source that emits nx::span<const char>).  After each non-empty feed,
// data_available is emitted so consumers can inspect the buffer.

class read_accumulator : public nx::core::object {
public:
    NX_OBJECT(read_accumulator)
    NX_DISABLE_COPY(read_accumulator)
    NX_DISABLE_MOVE(read_accumulator)

    explicit read_accumulator(nx::core::object * parent = nullptr);

    // ── Slot ──────────────────────────────────────────────────────────────────

    void feed(nx::span<const char> data);

    // ── Buffer access ─────────────────────────────────────────────────────────

    nx::span<const char> buffered()                                    const noexcept;
    bool                 empty()                                       const noexcept;
    void                 consume(std::size_t n);

    std::optional<nx::span<const char>> try_read_exactly(std::size_t n)      const noexcept;
    std::optional<nx::span<const char>> try_read_until  (char delim)          const noexcept;
    std::optional<nx::span<const char>> try_read_until  (std::string_view d)  const noexcept;

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(data_available)

private:
    read_buffer buf_;
};

} // namespace nx::network
