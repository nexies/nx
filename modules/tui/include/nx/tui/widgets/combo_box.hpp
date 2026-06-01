#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/animation/animated_value.hpp>
#include <nx/tui/types/color.hpp>

#include <string>
#include <vector>

namespace nx::tui {

// ── combo_box ─────────────────────────────────────────────────────────────────
//
// A drop-down selection widget.  Always occupies exactly 1 row in the layout.
// When expanded the dropdown is rendered as an overlay on top of other widgets
// via a second render pass in screen — the layout is never disturbed.
//
// Collapsed (height = 1):
//   ┌ Selected item text                          ▼ ┐
//
// Expanded (overlay below, no layout change):
//   ┌ Selected item text                          ▲ ┐  ← header (layout row)
//   │ Item 0                                        │  ← overlay
//   │ Item 1  ◀ selected                            │
//   │ Item 2                                        │
//
// Keyboard (collapsed): Enter / ↓ → expand.
// Keyboard (expanded):  ↑/↓ move highlight, Enter select, Escape cancel.

class combo_box : public widget {
    std::vector<std::string> items_;
    int current_       = -1;
    int highlight_     = -1;
    int saved_current_ = -1;
    bool expanded_     = false;
    int  max_visible_  = 6;

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

    // ── Overlay API ───────────────────────────────────────────────────────────

    [[nodiscard]] bool      has_overlay()  const noexcept override;
    [[nodiscard]] rect<int> overlay_rect() const noexcept override;

protected:
    void on_paint(painter & p)             override;
    void on_paint_overlay(painter & p)     override;
    void on_key_press(key_event & e)       override;
    void on_mouse_press(mouse_event & e)   override;
    void on_mouse_move(mouse_event & e)    override;
    void on_mouse_enter(mouse_event & e)   override;
    void on_mouse_leave(mouse_event & e)   override;
    void on_focus_in()                     override;
    void on_focus_out()                    override;

private:
    [[nodiscard]] int  _visible_count() const noexcept;

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
