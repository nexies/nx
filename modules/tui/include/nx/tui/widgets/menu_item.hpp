#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/animation/animated_value.hpp>
#include <nx/tui/types/color.hpp>

#include <string>

namespace nx::tui {

// ── menu_item ─────────────────────────────────────────────────────────────────
//
// A single selectable entry inside a menu.  Full widget — subclass to add
// icons, badges, or custom content; override on_paint() as needed.
//
// Visual states (all animated):
//   hover    — background brightens to control
//   selected — background switches to selection color
//
// menu_item does NOT take focus.  All keyboard navigation is owned by menu.
// Clicking emits clicked(); menu connects to this and drives set_current().

class menu_item : public widget {
    std::string           text_;
    bool                  selected_ = false;
    animated_value<color> bg_       { this, _normal_bg() };

public:
    NX_OBJECT(menu_item)

    explicit menu_item(nx::core::object * parent = nullptr);

    // ── Text ──────────────────────────────────────────────────────────────────

    void set_text(std::string t);
    [[nodiscard]] const std::string & text() const noexcept { return text_; }
    NX_PROPERTY(text, READ text, WRITE set_text)

    // ── Selection state (driven by menu) ──────────────────────────────────────

    void set_selected(bool s);
    [[nodiscard]] bool is_selected() const noexcept { return selected_; }

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(clicked)

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p)           override;
    void on_mouse_press(mouse_event & e) override;
    void on_mouse_enter(mouse_event & e) override;
    void on_mouse_leave(mouse_event & e) override;

private:
    void _update_bg();

    [[nodiscard]] color _normal_bg()   const noexcept;
    [[nodiscard]] color _hover_bg()    const noexcept;
    [[nodiscard]] color _selected_bg() const noexcept;
    [[nodiscard]] color _selected_fg() const noexcept;
    [[nodiscard]] color _normal_fg()   const noexcept;
    [[nodiscard]] color _target_bg()   const noexcept;
};

} // namespace nx::tui
