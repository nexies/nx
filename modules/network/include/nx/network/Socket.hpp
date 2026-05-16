#pragma once

#include <nx/network/types.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>
#include <nx/asio/backend/backend_types.hpp>

#include <nx/core2/io_device.hpp>
#include <nx/core2/thread/thread.hpp>

#include <chrono>
#include <memory>

// Forward-declare the internal impl so public headers stay clean.
namespace nx::network::detail { class socket_impl; }

namespace nx::network {

// ── socket ────────────────────────────────────────────────────────────────────
//
// Abstract base for all network sockets.  Extends nx::core::io_device so it
// participates in the object system (thread affinity, signals, properties).
//
// Async usage:
//   Connect ready_read / bytes_written signals (from io_device) to slots.
//
// Sync usage:
//   Call wait_for_read(timeout), then read().

class socket : public nx::core::io_device {
public:
    NX_OBJECT(socket)
    NX_DISABLE_COPY(socket)
    NX_DISABLE_MOVE(socket)

    enum class state {
        closed,
        open,
        bound,
        connecting,
        connected,
        listening,
    };

    // ── State ─────────────────────────────────────────────────────────────────

    NX_NODISCARD state socket_state() const noexcept;
    NX_NODISCARD bool  is_open()      const noexcept override;

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    // Creates the native socket and registers it with the current thread's
    // io_context.  Must be called from a thread that has an event loop.
    nx::result<void> open(socket_family family = socket_family::ipv4);
    void             close();

    // ── Sync wait ─────────────────────────────────────────────────────────────

    // Block until data is available or timeout expires.  timeout < 0 → forever.
    nx::result<void> wait_for_read(std::chrono::milliseconds timeout =
                                       std::chrono::milliseconds { -1 });

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(error_occurred, nx::error)

    NX_SIGNAL(state_changed,  state)

protected:
    explicit socket(socket_type type, nx::core::object * parent = nullptr);
    ~socket() override;

    // ── For subclasses ────────────────────────────────────────────────────────

    // Delivered by the platform impl; subclasses implement.
    virtual void _on_io_event(nx::asio::io_event ev) = 0;

    // Impl access (include detail/socket_impl.hpp in the calling TU).
    NX_NODISCARD detail::socket_impl       & _impl() noexcept;
    NX_NODISCARD const detail::socket_impl & _impl() const noexcept;

    // Transition to a new state and emit state_changed.
    void _set_state(state s);

    // Take ownership of a pre-built impl (e.g. accepted connection).
    // Caller must call _impl().attach() + _impl().arm_read() afterwards.
    void _adopt_impl(std::unique_ptr<detail::socket_impl> impl) noexcept;

    // core2 hook: re-register native handle on the new thread's io_context.
    void _on_thread_changed(nx::core::thread * old_t,
                             nx::core::thread * new_t) override;

private:
    socket_type                          type_;
    state                                state_ { state::closed };
    std::unique_ptr<detail::socket_impl> impl_;
};

} // namespace nx::network
