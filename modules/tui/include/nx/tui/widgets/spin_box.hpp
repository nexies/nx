#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/animation/animated_value.hpp>
#include <nx/tui/types/color.hpp>

#include <string>

namespace nx::tui {

// ── spin_box ──────────────────────────────────────────────────────────────────
//
// Numeric input widget (height = 1).
//
// Visual:
//   ┌ prefix  42  suffix ▲▼ ┐   ← focused
//   ┌ prefix  42  suffix    ┘   ← unfocused
//
// Keyboard: ↑/↓ increment/decrement, type digits directly, Enter to confirm.
// Mouse:    click ▲ column → increment, ▼ column → decrement.

class spin_box : public widget {
    int         value_ = 0;
    int         min_   = 0;
    int         max_   = 99;
    int         step_  = 1;
    std::string prefix_;
    std::string suffix_;

    animated_value<color> bg_ { this, _idle_bg() };

    // Direct digit entry
    bool        editing_  = false;
    std::string edit_buf_;

public:
    NX_OBJECT(spin_box)

    explicit spin_box(nx::core::object * parent = nullptr);

    // ── Range & value ─────────────────────────────────────────────────────────

    void set_range(int min, int max);
    void set_value(int v);
    void set_step  (int s)          noexcept { step_   = s; }
    void set_prefix(std::string s)           { prefix_ = std::move(s); update(); }
    void set_suffix(std::string s)           { suffix_ = std::move(s); update(); }

    [[nodiscard]] int                value()   const noexcept { return value_; }
    [[nodiscard]] int                minimum() const noexcept { return min_;   }
    [[nodiscard]] int                maximum() const noexcept { return max_;   }
    [[nodiscard]] const std::string& prefix()  const noexcept { return prefix_; }
    [[nodiscard]] const std::string& suffix()  const noexcept { return suffix_; }

    NX_PROPERTY(value, READ value, WRITE set_value)

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(value_changed, int)

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p)             override;
    void on_key_press(key_event & e)       override;
    void on_mouse_press(mouse_event & e)   override;
    void on_mouse_enter(mouse_event & e)   override;
    void on_mouse_leave(mouse_event & e)   override;
    void on_focus_in()                     override;
    void on_focus_out()                    override;

private:
    void _set_value_clamped(int v);
    void _commit_edit();
    void _cancel_edit();

    [[nodiscard]] std::string _display_text() const;

    [[nodiscard]] color _idle_bg()  const noexcept;
    [[nodiscard]] color _hover_bg() const noexcept;
    [[nodiscard]] color _idle_fg()  const noexcept;
    [[nodiscard]] color _accent()   const noexcept;
};

} // namespace nx::tui
