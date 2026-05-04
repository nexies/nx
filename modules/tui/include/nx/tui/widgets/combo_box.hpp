#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/animation/animated_value.hpp>
#include <nx/tui/types/color.hpp>

#include <string>
#include <vector>

namespace nx::tui {

// ── combo_box ─────────────────────────────────────────────────────────────────
//
// A drop-down selection widget that expands inline (no overlay required).
//
// Collapsed (height = 1):
//   ┌ Selected item text                          ▼ ┐
//
// Expanded (height = 1 + min(count, max_visible)):
//   ┌ Selected item text                          ▼ ┐  ← header
//   │ Item 0                                        │  ← dropdown
//   │ Item 1  ◀ selected                            │
//   │ Item 2                                        │
//   └───────────────────────────────────────────────┘
//
// When expanded the parent layout re-flows automatically (set_geometry replay).
//
// Keyboard (collapsed): Enter / Space → expand.
// Keyboard (expanded):  ↑/↓ move highlight, Enter select, Escape cancel.

class combo_box : public widget {
    std::vector<std::string> items_;
    int current_     = -1;
    int highlight_   = -1;  // index highlighted while dropdown is open
    int saved_current_ = -1;
    bool expanded_   = false;
    int  max_visible_ = 6;

    animated_value<color> bg_     { this, _idle_bg() };
    animated_value<color> border_ { this, _border_c() };

public:
    NX_OBJECT(combo_box)

    explicit combo_box(nx::core::object * parent = nullptr);

    // ── Items ─────────────────────────────────────────────────────────────────

    void add_item(std::string text);
    void set_items(std::vector<std::string> items);
    void remove_item(int index);
    void clear();

    [[nodiscard]] int                count()          const noexcept;
    [[nodiscard]] const std::string& item_text(int i) const;

    void set_max_visible(int n) noexcept { max_visible_ = n; }

    // ── Selection ─────────────────────────────────────────────────────────────

    void set_current(int index);
    [[nodiscard]] int current() const noexcept { return current_; }

    // ── State ─────────────────────────────────────────────────────────────────

    [[nodiscard]] bool is_expanded() const noexcept { return expanded_; }
    void expand();
    void collapse(bool accept = true);

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(current_changed, int)
    NX_SIGNAL(activated, int)

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p)             override;
    void on_key_press(key_event & e)       override;
    void on_mouse_press(mouse_event & e)   override;
    void on_mouse_move(mouse_event & e)    override;
    void on_mouse_enter(mouse_event & e)   override;
    void on_mouse_leave(mouse_event & e)   override;
    void on_focus_in()                     override;
    void on_focus_out()                    override;

private:
    [[nodiscard]] int  _visible_count()    const noexcept;
    void               _set_height(int h);
    void               _request_parent_layout();

    [[nodiscard]] color _idle_bg()   const noexcept;
    [[nodiscard]] color _hover_bg()  const noexcept;
    [[nodiscard]] color _idle_fg()   const noexcept;
    [[nodiscard]] color _accent()    const noexcept;
    [[nodiscard]] color _border_c()  const noexcept;
    [[nodiscard]] color _sel_bg()    const noexcept;
    [[nodiscard]] color _sel_fg()    const noexcept;
    [[nodiscard]] color _hl_bg()     const noexcept;
};

} // namespace nx::tui
