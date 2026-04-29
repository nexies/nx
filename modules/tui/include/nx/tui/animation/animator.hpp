#pragma once

#include <nx/core2/object/object.hpp>
#include <nx/tui/animation/easing.hpp>

#include <chrono>
#include <functional>

namespace nx::tui {

class animation_manager;

// ── animator ──────────────────────────────────────────────────────────────────
//
// A float value [0..1] that smoothly animates toward a target.
//
// Calling animate_to() while an animation is in progress always starts from
// the current value — no jumps, no restarts.
//
// If proportional=true (default), the actual duration is scaled by the
// remaining distance (|target - value|), keeping visual speed constant across
// interrupted animations.
//
// Typically owned by a widget or animated_value<T>.  Registers itself with
// the application's animation_manager on start; unregisters on finish or
// destruction.
//
// Usage:
//   animator * a = new animator(this);   // parent = owning widget
//   a->animate_to(1.0f, 200);
//   // in on_paint: float t = a->value();

class animator : public nx::core::object {
    using clock_t    = std::chrono::steady_clock;
    using time_point = clock_t::time_point;

    float      value_    = 0.0f;
    float      from_     = 0.0f;
    float      target_   = 0.0f;
    int        duration_ = 0;      // effective duration in ms
    easing_fn  easing_   = easing::linear;
    bool       active_   = false;
    time_point start_time_;

    std::function<void(float)> on_update_;

public:
    explicit animator(nx::core::object * parent = nullptr);
    ~animator() override;

    // ── Animation control ─────────────────────────────────────────────────────

    // Interrupt or start: always animates from current value_ to target.
    // proportional=true: duration scaled by |target - value_| so visual
    // speed stays constant when interrupting mid-animation.
    void animate_to(float           target,
                    int             duration_ms,
                    easing_fn       fn           = easing::ease_out,
                    bool            proportional = true);

    // Jump immediately to v; cancels any running animation.
    void set_value(float v) noexcept;

    // ── Callback ──────────────────────────────────────────────────────────────

    // Called on every tick with the new value.  Fired before the parent widget
    // receives its update() call, so animated_value<T> can update current_ first.
    void set_update(std::function<void(float)> fn);

    // ── Queries ───────────────────────────────────────────────────────────────

    [[nodiscard]] float value()        const noexcept { return value_; }
    [[nodiscard]] bool  is_animating() const noexcept { return active_; }

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(finished)

private:
    friend class animation_manager;

    // Advance one frame.  Returns false when the animation is complete.
    bool _tick(time_point now);

    void _register();
    void _unregister();
};

} // namespace nx::tui
