#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/types/margin.hpp>

#include <vector>

namespace nx::tui {

// ── layout ────────────────────────────────────────────────────────────────────
//
// Abstract base for all ordered-container widgets (v_box, h_box, v_stack,
// h_stack, a_box).
//
// Manages the ordered items_ list and exposes the common add / insert / remove /
// spacing / margin API.  Subclasses implement _apply_layout() and minimum_size().
//
// Children added via their own constructor (new label(box)) are picked up
// automatically by _sync_items(), which subclasses call at the top of their
// _apply_layout() override.

class layout : public widget {
public:
    NX_OBJECT(layout)

    void set_spacing(int s) noexcept { spacing_ = s; }
    [[nodiscard]] int spacing() const noexcept { return spacing_; }

    void set_margin(const nx::tui::margin & m) noexcept { margin_ = m; }
    void set_margin(int all)                   noexcept { margin_ = nx::tui::margin{all}; }
    [[nodiscard]] const nx::tui::margin & margin() const noexcept { return margin_; }

    // Append w (reparenting if necessary).
    void add(widget * w);

    // Insert w at position idx (0-based).
    void insert(int idx, widget * w);

    // Insert w immediately before `before`; appends if `before` is not found.
    void insert_before(widget * before, widget * w);

    // Remove w from this layout and reparent to nullptr.
    void remove(widget * w);

    [[nodiscard]] const std::vector<widget *> & items() const noexcept { return items_; }

protected:
    explicit layout(nx::core::object * parent = nullptr);

    std::vector<widget *> items_;
    int                   spacing_ = 0;
    nx::tui::margin       margin_  {};

    // Re-sync items_ with the current child_widgets() list.
    // Call at the start of _apply_layout() to pick up children added via
    // their constructor before items_ was populated.
    void _sync_items();
};

} // namespace nx::tui
