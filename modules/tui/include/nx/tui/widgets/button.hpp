#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/animation/animated_value.hpp>
#include <nx/tui/types/color.hpp>
#include <nx/tui/types/border_style.hpp>
#include <nx/tui/types/theme_role.hpp>

#include <optional>
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

    // ── Color overrides (nullopt = derive from application theme) ────────────
    std::optional<color> base_bg_override_;
    std::optional<color> hover_bg_override_;
    std::optional<color> press_bg_override_;
    std::optional<color> base_fg_override_;
    std::optional<color> accent_override_;

    // Whether animated values have been initialised from the theme at least once.
    bool colors_initialized_ = false;

    // ── Animation state ───────────────────────────────────────────────────────
    animated_value<color> bg_     { this, color::default_color };
    animated_value<color> border_ { this, color::default_color };

    border_style border_style_ = border_style::rounded;

public:
    NX_OBJECT(button)

    explicit button(nx::core::object * parent = nullptr);

    // ── Text ──────────────────────────────────────────────────────────────────

    void set_text(std::string t);
    [[nodiscard]] const std::string & text() const noexcept { return text_; }

    NX_PROPERTY(text, READ text, WRITE set_text)

    // ── Palette ───────────────────────────────────────────────────────────────

    // Override base bg and fg; hover/press variants are computed automatically
    // (15 % and 30 % white mix).  Calling this disables theme-based colors.
    // Pass color::default_color to re-enable theme lookup for that slot.
    void set_base_color(color bg, color fg = color::default_color);

    void set_accent_color(color c) { accent_override_ = c; }
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

    // Resolve colors — use explicit override if set, otherwise read from theme.
    [[nodiscard]] color _resolve_idle_bg()   const noexcept;
    [[nodiscard]] color _resolve_hover_bg()  const noexcept;
    [[nodiscard]] color _resolve_press_bg()  const noexcept;
    [[nodiscard]] color _resolve_idle_fg()   const noexcept;
    [[nodiscard]] color _resolve_accent()    const noexcept;
    [[nodiscard]] color _target_bg()         const noexcept;

    // Initialise animated values from the theme on first paint.
    void _init_colors() noexcept;
};

} // namespace nx::tui
