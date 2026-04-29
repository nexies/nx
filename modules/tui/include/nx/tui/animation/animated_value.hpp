#pragma once

#include <nx/tui/animation/animator.hpp>
#include <nx/tui/animation/interpolate.hpp>

namespace nx::tui {

// ── animated_value<T> ─────────────────────────────────────────────────────────
//
// A typed value that smoothly animates between states.
// Wraps an animator (0→1 float) and calls interpolate<T>() each tick.
//
// Interruption is seamless: animate_to() always starts from current value,
// so mid-animation reversals produce no jumps.
//
// Usage:
//   animated_value<color> bg_{this, color::dark};
//
//   void on_mouse_enter(mouse_event &) override {
//       bg_.animate_to(color::accent, 150);
//   }
//   void on_mouse_leave(mouse_event &) override {
//       bg_.animate_to(color::dark, 150);
//   }
//   void on_paint(painter & p) override {
//       p.apply_style(bg(bg_.value()));
//       ...
//   }

template <typename T>
class animated_value {
    T        from_;
    T        to_;
    T        current_;
    animator anim_;

public:
    explicit animated_value(nx::core::object * parent, T initial = T{})
        : from_(initial), to_(initial), current_(initial)
        , anim_(parent)
    {
        anim_.set_update([this](float t) {
            current_ = interpolate<T>(t, from_, to_);
        });
    }

    // ── Animation control ─────────────────────────────────────────────────────

    void animate_to(T           target,
                    int         duration_ms,
                    easing_fn   fn = easing::ease_out)
    {
        from_ = current_;
        to_   = target;
        // Reset underlying float to 0 so interpolate() maps 0→from_, 1→to_.
        // Disable proportional here — duration is specified in typed units by
        // the caller; proportional scaling at the float level would distort it.
        anim_.set_value(0.0f);
        anim_.animate_to(1.0f, duration_ms, fn, /*proportional=*/false);
    }

    // Jump immediately without animation.
    void set(T v)
    {
        from_ = to_ = current_ = v;
        anim_.set_value(0.0f);
    }

    // ── Queries ───────────────────────────────────────────────────────────────

    [[nodiscard]] const T & value()        const noexcept { return current_; }
    [[nodiscard]] bool      is_animating() const noexcept { return anim_.is_animating(); }

    // Access the underlying animator (e.g. to connect to finished signal).
    [[nodiscard]] animator &       raw()       noexcept { return anim_; }
    [[nodiscard]] const animator & raw() const noexcept { return anim_; }
};

} // namespace nx::tui
