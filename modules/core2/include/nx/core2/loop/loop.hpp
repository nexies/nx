//
// loop — event loop tied to a thread's io_context.
//
// Typical usage:
//   nx::core::loop l;
//   int code = l.exec();   // blocks until exit() or quit() is called
//
// A thread can have nested loops (e.g. a modal dialog pumping events).
// The innermost loop is always accessible via loop::current().
//

#pragma once

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>

#include <nx/core2/object/object.hpp>

#include <atomic>
#include <chrono>
#include <optional>

#include "nx/asio/backend/backend_types.hpp"

namespace nx::core {

class thread;

// ──────────────────────────────────────────────────────────────────────────────
// loop
// ──────────────────────────────────────────────────────────────────────────────

class loop : public object {
public:
    explicit loop(object * parent = nullptr);
    ~loop() override;

    NX_DISABLE_COPY(loop)
    NX_OBJECT(loop)

    // ── Blocking execution ────────────────────────────────────────────────────

    // Run the event loop until exit() or quit() is called.
    // Blocks the calling thread. Returns the exit code.
    int
    exec();

    // Process all pending events and return (non-blocking).
    // Returns the number of tasks executed.
    std::size_t
    process_events();

    // Process events for at most `duration`.
    std::size_t
    process_events_for(nx::asio::duration duration);

    // ── Control ───────────────────────────────────────────────────────────────

    // Stop the loop with exit code 0.
    void
    quit();

    // Stop the loop with the given exit code.
    void
    exit(int code = 0);

    // Wake up the loop if it is sleeping (useful from other threads).
    void
    wakeup();

    // ── State ─────────────────────────────────────────────────────────────────

    NX_NODISCARD bool
    is_running() const noexcept;

    NX_NODISCARD int
    exit_code() const noexcept;

    // ── Signals ───────────────────────────────────────────────────────────────

    // Emitted when the loop is about to exit (before exec() returns).
    NX_SIGNAL(about_to_quit)

    // ── Static ────────────────────────────────────────────────────────────────

    // The innermost loop running on the calling thread. Null if none.
    NX_NODISCARD static loop *
    current() noexcept;

private:
    void _install_on_thread(thread * t);
    void _uninstall_from_thread(thread * t);

    std::atomic_bool running_    { false };
    std::atomic_bool stop_requested_ { false };
    std::atomic_int  exit_code_  { 0 };
};

} // namespace nx::core
