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
//   hover  — background brightens
//   focus  — label text becomes bold
//   press  — background brightens further; border flashes to accent color
//
// Colors come exclusively from the application theme.
// To customise appearance, subclass and override on_paint().

class button : public widget {
    std::string text_;

    // ── Animation state ───────────────────────────────────────────────────────
    animated_value<color> bg_     { this, _idle_bg() };
    animated_value<color> border_ { this, _idle_fg() };

    border_style border_style_ = border_style::rounded;

public:
    NX_OBJECT(button)

    explicit button(nx::core::object * parent = nullptr);

    // ── Text ──────────────────────────────────────────────────────────────────

    void set_text(std::string t);
    [[nodiscard]] const std::string & text() const noexcept { return text_; }

    NX_PROPERTY(text, READ text, WRITE set_text)

    // ── Appearance ────────────────────────────────────────────────────────────

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

    // Theme color lookups — hardcoded fallbacks when no app exists.
    [[nodiscard]] color _idle_bg () const noexcept;
    [[nodiscard]] color _hover_bg() const noexcept;
    [[nodiscard]] color _press_bg() const noexcept;
    [[nodiscard]] color _idle_fg () const noexcept;
    [[nodiscard]] color _accent  () const noexcept;
    [[nodiscard]] color _target_bg() const noexcept;
};

} // namespace nx::tui
