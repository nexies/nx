#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/animation/animated_value.hpp>
#include <nx/tui/types/color.hpp>
#include <nx/tui/types/border_style.hpp>

#include <string>

namespace nx::tui {

// ── button ────────────────────────────────────────────────────────────────────
//
// Clickable widget with an animated rounded border.
//
// Visual states (all animated):
//   hover  — background brightens (bg mixed toward white)
//   focus  — label text becomes bold
//   press  — background brightens further; border flashes to accent color
//
// The default palette can be overridden with set_base_color() / set_accent_color().
// Default size: width flexible, height 3 (border + 1 content row + border).

class button : public widget {
    std::string text_;

    // ── Palette ───────────────────────────────────────────────────────────────
    color base_bg_     = color::black;
    color hover_bg_    = color::rgb( 81,  81,  98);  // base + 15 % white
    color press_bg_    = color::rgb(112, 112, 126);  // base + 30 % white
    color base_fg_     = color::rgb(220, 220, 240);
    color accent_      = color::rgb(255, 180,  60);

    // ── Animation state ───────────────────────────────────────────────────────
    animated_value<color> bg_     { this, base_bg_  };
    animated_value<color> border_ { this, base_fg_  };

    border_style border_style_ = border_style::rounded;

public:
    NX_OBJECT(button)

    explicit button(nx::core::object * parent = nullptr);

    // ── Text ──────────────────────────────────────────────────────────────────

    void set_text(std::string t);
    [[nodiscard]] const std::string & text() const noexcept { return text_; }

    NX_PROPERTY(text, READ text, WRITE set_text)

    // ── Palette ───────────────────────────────────────────────────────────────

    // Set base bg and fg; hover/press variants are computed automatically
    // (15 % and 30 % white mix respectively).
    void set_base_color(color bg, color fg = color::rgb(220, 220, 240));

    void set_accent_color(color c) { accent_ = c; }
    void set_border_style(border_style s) { border_style_ = s; update(); }

    // ── Signals ───────────────────────────────────────────────────────────────

    void click();
    NX_SIGNAL(clicked)

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p)             override;
    void on_focus_in()                     override;
    void on_focus_out()                    override;
    void on_mouse_enter(mouse_event & e)   override;
    void on_mouse_leave(mouse_event & e)   override;
    void on_mouse_press(mouse_event & e)   override;
    void on_mouse_release(mouse_event & e) override;
    void on_key_press(key_event & e)       override;

private:
    void _do_click();
    [[nodiscard]] color _target_bg() const noexcept;
};

} // namespace nx::tui
