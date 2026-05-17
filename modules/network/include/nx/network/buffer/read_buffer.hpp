#pragma once

#include <nx/common/span.hpp>

#include <cstddef>
#include <optional>
#include <string_view>
#include <vector>

namespace nx::network {

// ── read_buffer ───────────────────────────────────────────────────────────────
//
// Linear byte buffer for accumulating incoming data.
//
// Write side: prepare(hint) → get writable span; commit(n) → mark bytes filled.
// Read side:  data() → view of available bytes; consume(n) → advance past them.
//
// Memory is compacted automatically when the read head passes the midpoint.
// The buffer grows on demand when prepare() cannot satisfy the request.

class read_buffer {
public:
    static constexpr std::size_t default_capacity = 4096;

    explicit read_buffer(std::size_t initial_capacity = default_capacity);

    // ── Read side ─────────────────────────────────────────────────────────────

    nx::span<const char> data() const noexcept;

    std::size_t size()  const noexcept;
    bool        empty() const noexcept;

    void consume(std::size_t n);

    std::optional<nx::span<const char>> try_read_exactly(std::size_t n)     const noexcept;
    std::optional<nx::span<const char>> try_read_until  (char delim)         const noexcept;
    std::optional<nx::span<const char>> try_read_until  (std::string_view d) const noexcept;

    // ── Write side ────────────────────────────────────────────────────────────

    nx::span<char> prepare(std::size_t hint = default_capacity);
    void           commit (std::size_t n);

    // ── Misc ──────────────────────────────────────────────────────────────────

    void        clear()    noexcept;
    std::size_t capacity() const noexcept;

private:
    void _compact();
    void _grow(std::size_t min_capacity);

    std::vector<char> buf_;
    std::size_t       head_ { 0 };
    std::size_t       tail_ { 0 };
};

} // namespace nx::network
