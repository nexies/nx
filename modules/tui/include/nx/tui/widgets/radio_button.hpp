#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/animation/animated_value.hpp>
#include <nx/tui/types/color.hpp>

#include <string>
#include <vector>

namespace nx::tui {

class radio_button;

// ── radio_group ───────────────────────────────────────────────────────────────
//
// Non-visual controller that enforces mutual exclusion among radio_buttons.
// Does NOT take ownership of buttons; caller is responsible for lifetime.
//
// Usage:
//   auto * grp = new radio_group(parent);
//   grp->add(btn_a);
//   grp->add(btn_b);
//   grp->set_current(0);   // selects btn_a

class radio_group : public nx::core::object {
    std::vector<radio_button *> buttons_;
    int current_ = -1;

public:
    NX_OBJECT(radio_group)

    explicit radio_group(nx::core::object * parent = nullptr);

    void add(radio_button * btn);
    void set_current(int idx);

    [[nodiscard]] int current() const noexcept { return current_; }
    [[nodiscard]] int count()   const noexcept { return static_cast<int>(buttons_.size()); }

    NX_SIGNAL(selection_changed, int)

    friend class radio_button;

private:
    void _button_selected(radio_button * src);
};

// ── radio_button ──────────────────────────────────────────────────────────────
//
// Single-row mutually exclusive toggle:
//
//   ◯ Label     (unchecked)
//   ◉ Label     (checked — dot fades in with highlight color)
//
// Can be used standalone (simple on/off) or inside a radio_group.
// Keyboard: Space / Enter to select.
// Mouse:    Click anywhere on the widget to select.

class radio_button : public widget {
    std::string  text_;
    bool         checked_     = false;
    bool         initialized_ = false;
    radio_group * group_      = nullptr;

    animated_value<color> dot_color_ { this, color::default_color };

public:
    NX_OBJECT(radio_button)

    explicit radio_button(nx::core::object * parent = nullptr);

    // ── Content ───────────────────────────────────────────────────────────────

    void set_text(std::string t);
    [[nodiscard]] const std::string & text() const noexcept { return text_; }

    NX_PROPERTY(text, READ text, WRITE set_text)

    // ── State ─────────────────────────────────────────────────────────────────

    void set_checked(bool c);
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
    // void _init_colors() noexcept;
    // Called by radio_group to forcibly deselect without re-notifying the group.
    void _deselect();

    color _active_dot_color() noexcept;
    color _inactive_dot_color() noexcept;

    friend class radio_group;
};

} // namespace nx::tui
