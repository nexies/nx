#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/animation/animated_value.hpp>

#include <string>

namespace nx::tui {

// ── spinner ───────────────────────────────────────────────────────────────────
//
// Animated loading indicator — cycles through braille frames:
//   ⠋ ⠙ ⠹ ⠸ ⠼ ⠴ ⠦ ⠧ ⠇ ⠏
//
// Fixed height = 1.  Width = 1 (glyph only) or 2 + label.length() when a label
// is set ("⠋ Loading…").
//
// start() / stop() control the animation.  The spinner is stopped by default.

class spinner : public widget {
    static constexpr int k_frame_count = 10;
    static constexpr int k_cycle_ms    = k_frame_count * 80; // 800 ms per cycle

    animated_value<float> tick_ { this, 0.0f };
    bool        spinning_ = false;
    std::string label_;

public:
    NX_OBJECT(spinner)

    explicit spinner(nx::core::object * parent = nullptr);

    void start();
    void stop();
    [[nodiscard]] bool is_spinning() const noexcept { return spinning_; }

    void set_label(std::string l) { label_ = std::move(l); update(); }
    [[nodiscard]] const std::string & label() const noexcept { return label_; }

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p) override;

private:
    [[nodiscard]] int _current_frame() const noexcept;
    void _next_cycle();
};

} // namespace nx::tui
