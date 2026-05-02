#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/animation/animated_value.hpp>
#include <nx/tui/types/theme_role.hpp>

#include <string>
#include <vector>

namespace nx::tui {

// ── tab_bar ───────────────────────────────────────────────────────────────────
//
// Single-row horizontal strip of tab labels.
//
// The active tab is drawn with theme highlight color + underline style.
// The underline indicator is an animated window [ind_x_, ind_x_+ind_w_) that
// slides and resizes between tabs on every set_current() call.
//
// Each tab slot occupies  1 (padding) + label_width + 1 (padding)  columns.
// Tabs are placed contiguously with no extra gap (padding acts as separator).

class tab_bar : public widget {
    std::vector<std::string> tabs_;
    int  current_     = -1;
    bool initialized_ = false;

    // Animated underline: x offset and width in screen columns.
    animated_value<int> ind_x_ { this, 0 };
    animated_value<int> ind_w_ { this, 0 };

public:
    NX_OBJECT(tab_bar)

    explicit tab_bar(nx::core::object * parent = nullptr);

    // ── Tab management ────────────────────────────────────────────────────────

    void add_tab(std::string title);
    void remove_tab(int idx);
    void set_tab_title(int idx, std::string title);

    [[nodiscard]] int count()   const noexcept { return static_cast<int>(tabs_.size()); }
    [[nodiscard]] int current() const noexcept { return current_; }

    [[nodiscard]] const std::string & tab_title(int idx) const;

    void set_current(int idx);

    NX_SIGNAL(current_changed, int)

    [[nodiscard]] size_type size_hint() const override;

protected:
    void on_paint(painter & p)           override;
    void on_mouse_press(mouse_event & e) override;
    void on_key_press(key_event & e)     override;
    void on_focus_in()                   override;
    void on_focus_out()                  override;

private:
    // Column where tab idx begins (0-based).
    [[nodiscard]] int _tab_x(int idx) const noexcept;
    // Width in columns of tab idx (padding + label + padding).
    [[nodiscard]] int _tab_w(int idx) const noexcept;
    // Tab index whose slot contains column col; -1 if none.
    [[nodiscard]] int _tab_at_col(int col) const noexcept;

    // Snap indicator to current_ immediately (no animation).
    void _snap_indicator() noexcept;
    // Animate indicator to current_ position.
    void _animate_indicator() noexcept;
};

} // namespace nx::tui
