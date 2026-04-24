//
// io_device — abstract base for stream-based I/O (files, sockets, pipes, …).
//

#pragma once

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>

#include <nx/core2/object/object.hpp>

#include <cstdint>

namespace nx::core {

// ──────────────────────────────────────────────────────────────────────────────
// io_device
// ──────────────────────────────────────────────────────────────────────────────

class io_device : public object {
public:
    explicit io_device(object * parent = nullptr);
    ~io_device() override;

    NX_OBJECT(io_device)

    // ── Sync I/O ──────────────────────────────────────────────────────────────

    NX_NODISCARD virtual nx::result<std::size_t>
    read(char * buf, std::size_t max_len) = 0;

    NX_NODISCARD virtual nx::result<std::size_t>
    write(const char * buf, std::size_t len) = 0;

    // ── State ─────────────────────────────────────────────────────────────────

    NX_NODISCARD virtual bool
    is_open() const noexcept = 0;

    // ── Signals ───────────────────────────────────────────────────────────────

    // Emitted when new data is available for reading.
    NX_SIGNAL(ready_read)

    // Emitted after data has been written; carries the number of bytes written.
    NX_SIGNAL(bytes_written, std::uint64_t)

    // Emitted when a specific number of bytes is ready to be read.
    NX_SIGNAL(ready_read_bytes, std::uint64_t)
};

} // namespace nx::core
