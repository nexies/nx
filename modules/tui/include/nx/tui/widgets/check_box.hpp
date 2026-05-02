#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/animation/animated_value.hpp>
#include <nx/tui/types/color.hpp>

#include <string>

namespace nx::tui {

// ── check_box ─────────────────────────────────────────────────────────────────
//
// Single-row toggle widget rendered as:
//
//   [ ] Label text      (unchecked)
//   [✓] Label text      (checked — checkmark fades in with highlight color)
//
// Keyboard: Space / Enter to toggle.
// Mouse:    Click anywhere on the widget to toggle.

class check_box : public widget {
    std::string text_;
    bool        checked_     = false;
    bool        initialized_ = false;

    // Checkmark fg: animates background→highlight on check, reverse on uncheck.
    animated_value<color> check_color_ { this, color::default_color };

public:
    NX_OBJECT(check_box)

    explicit check_box(nx::core::object * parent = nullptr);

    // ── Content ───────────────────────────────────────────────────────────────

    void set_text(std::string t);
    [[nodiscard]] const std::string & text() const noexcept { return text_; }

    NX_PROPERTY(text, READ text, WRITE set_text)

    // ── State ─────────────────────────────────────────────────────────────────

    void set_checked(bool c);
    void toggle() { set_checked(!checked_); }
    [[nodiscard]] bool is_checked() const noexcept { return checked_; }

    NX_SIGNAL(toggled, bool)
    NX_PROPERTY(checked, READ is_checked, WRITE set_checked)

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p)           override;
    void on_key_press(key_event & e)     override;
    void on_mouse_press(mouse_event & e) override;
    void on_focus_in()                   override;
    void on_focus_out()                  override;

private:
    void _init_colors() noexcept;
};

} // namespace nx::tui
