#pragma once

#include <nx/network/types.hpp>
#include <nx/asio/backend/backend_types.hpp>
#include <nx/asio/context/io_context.hpp>
#include <nx/common/types/errors/codes.hpp>

#include <chrono>
#include <functional>
#include <memory>

namespace nx::network::detail {

// ── socket_impl ───────────────────────────────────────────────────────────────
//
// Platform socket abstraction.
//
//   POSIX: non-blocking fd + reactor_handle (epoll / kevent)
//   Win:   non-blocking SOCKET + WSAEventSelect + reactor_handle (IOCP waitable)
//
// Lifetime:
//   create()  — allocate native socket
//   attach()  — register with io_context for async events
//   detach()  — unregister (call before destroy or move_to_thread)
//   destroy() — close native socket
//
// The accept() factory produces a new impl with a valid socket but NOT yet
// attached. The caller must call attach() before receiving async events.

class socket_impl {
public:
    using event_cb = std::function<void(nx::asio::io_event)>;

    virtual ~socket_impl() = default;

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    virtual nx::result<void> create (socket_family family, socket_type type) = 0;
    virtual void             destroy() noexcept = 0;

    virtual void             attach (nx::asio::io_context & ctx, event_cb cb) = 0;
    virtual void             detach () noexcept = 0;

    // ── Async arming ──────────────────────────────────────────────────────────

    // Re-arm to receive the next read event (one-shot, call again from handler).
    virtual void arm_read() = 0;
    // Add write-readiness monitoring (call when write would block).
    virtual void arm_write() = 0;
    // Remove write-readiness monitoring (call after write queue is drained).
    virtual void disarm_write() = 0;

    // ── Socket ops ────────────────────────────────────────────────────────────

    virtual nx::result<void> bind   (const endpoint & ep) = 0;
    virtual nx::result<void> listen (int backlog) = 0;
    // Non-blocking connect — completion signalled via arm_write callback.
    virtual nx::result<void> connect(const endpoint & ep) = 0;

    // ── Convenience options (impl-layer only) ─────────────────────────────────

    virtual nx::result<void> set_reuse_address(bool enable) = 0;
    virtual nx::result<void> set_no_delay(bool enable) = 0;

    // ── Non-blocking I/O ──────────────────────────────────────────────────────

    virtual nx::result<std::size_t> read     (char * buf, std::size_t len) = 0;
    virtual nx::result<std::size_t> write    (const char * buf, std::size_t len) = 0;
    virtual nx::result<std::size_t> read_from(char * buf, std::size_t len, endpoint & from) = 0;
    virtual nx::result<std::size_t> write_to (const char * buf, std::size_t len,
                                              const endpoint & to) = 0;

    // ── Accept ────────────────────────────────────────────────────────────────

    // Returns a new impl wrapping the accepted socket (NOT yet attached).
    // Returns would_block_error() if no connection is pending.
    virtual nx::result<std::unique_ptr<socket_impl>> accept(endpoint & remote_ep) = 0;

    // ── Sync wait ─────────────────────────────────────────────────────────────

    // Block until socket is readable or timeout expires.
    virtual nx::result<void> wait_readable(std::chrono::milliseconds timeout) = 0;

    // ── State ─────────────────────────────────────────────────────────────────

    NX_NODISCARD virtual nx::asio::native_handle_t native_handle() const noexcept = 0;
    NX_NODISCARD virtual bool                      is_open()       const noexcept = 0;

    // ── Factory ───────────────────────────────────────────────────────────────

    static std::unique_ptr<socket_impl> make();

    // ── Error helpers ─────────────────────────────────────────────────────────

    static nx::err::runtime_error would_block_error();
    static bool      is_would_block(const nx::error & e) noexcept;
};

} // namespace nx::network::detail
