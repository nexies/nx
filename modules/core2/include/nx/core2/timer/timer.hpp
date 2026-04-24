//
// timer — one-shot or periodic timer backed by nx::asio::steady_timer.
//

#pragma once

#include <nx/common/helpers.hpp>
#include <nx/common/types/timing.hpp>

#include <nx/asio/steady_timer.hpp>
#include <nx/asio/context/io_context.hpp>

#include <nx/core2/object/object.hpp>

#include <chrono>
#include <memory>

namespace nx::core {

// ──────────────────────────────────────────────────────────────────────────────
// timer
// ──────────────────────────────────────────────────────────────────────────────

class timer : public object {
public:
    using duration_t   = nx::asio::duration;
    using time_point_t = nx::asio::time_point;

    enum class timer_type { single_shot, periodic };

    explicit timer(object * parent = nullptr);
    explicit timer(duration_t interval, timer_type type = timer_type::single_shot,
                   object * parent = nullptr);
    ~timer() override;

    NX_DISABLE_COPY(timer)
    NX_OBJECT(timer)

    // ── Control ───────────────────────────────────────────────────────────────

    // Start / restart the timer with the current interval.
    void start();

    // Start with a new interval (also updates the stored interval).
    void start(duration_t interval);

    // Stop the timer. Pending timeout signals are cancelled.
    void stop();

    NX_NODISCARD bool
    is_active() const noexcept;

    // ── Properties ────────────────────────────────────────────────────────────

    NX_PROPERTY(interval, TYPE duration_t,
                READ, WRITE, NOTIFY, DEFAULT std::chrono::seconds(1))

    NX_PROPERTY(type, TYPE timer_type,
                READ, WRITE, NOTIFY, DEFAULT timer_type::single_shot)

    // ── Signals ───────────────────────────────────────────────────────────────

    // Emitted each time the timer fires.
    NX_SIGNAL(timeout)

    // ── Static convenience ────────────────────────────────────────────────────

    // Create a one-shot timer that calls `callback` once after `interval`.
    // The timer (and its connection) is owned by `parent`.
    static timer *
    single_shot(duration_t interval, object * parent, std::function<void()> callback);

protected:
    void _on_thread_changed(thread * old_thread, thread * new_thread) override;

private:
    void _arm();
    void _on_timeout();
    void _invoke_callback();

    std::unique_ptr<nx::asio::steady_timer> asio_timer_;
    std::atomic_bool         active_    { false };
    std::function<void()>    callback_;
};

} // namespace nx::core
