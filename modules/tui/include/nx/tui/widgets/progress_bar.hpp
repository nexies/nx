#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/animation/animated_value.hpp>

namespace nx::tui {

// ── progress_bar ──────────────────────────────────────────────────────────────
//
// Horizontal progress indicator, fixed height = 1.
//
// Visual (width W):
//   [████████░░░░░░░░]  42%
//    ^filled^  ^empty^
//
// Filled cells: theme control_active background.
// Empty  cells: theme background_alt background.
// Percentage text is drawn right-aligned inside the bar when show_text=true.
//
// set_value() animates smoothly (300 ms, ease_out).
// set_value(v, false) snaps immediately.

class progress_bar : public widget {
    animated_value<float> value_ { this, 0.0f };
    float target_    = 0.0f;
    bool  show_text_ = true;

public:
    NX_OBJECT(progress_bar)

    explicit progress_bar(nx::core::object * parent = nullptr);

    void  set_value(float v, bool animate = true);
    [[nodiscard]] float value() const noexcept { return value_.value(); }

    void set_show_text(bool s) { show_text_ = s; update(); }
    [[nodiscard]] bool show_text() const noexcept { return show_text_; }

    NX_SIGNAL(value_changed, float)

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p) override;
};

} // namespace nx::tui
