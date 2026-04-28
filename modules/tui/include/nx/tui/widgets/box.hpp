#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/types/size_policy.hpp>
#include <nx/tui/types/margin.hpp>

#include <vector>

namespace nx::tui {

// ── v_box ─────────────────────────────────────────────────────────────────────
//
// Container widget that stacks its children vertically.
//
// Children are tracked in `items_` which is kept in sync with the object-tree
// child list via on_child_added / on_child_removed hooks.
//
//   new label(vbox)              → appended to items_ automatically
//   vbox->add(existing_widget)   → reparents to vbox, appended to items_
//   vbox->insert(0, w)           → reparents, placed at given index
//   vbox->insert_before(ref, w)  → reparents, placed before `ref`
//   vbox->remove(w)              → removes from items_ and reparents to nullptr
//
// Sizing:
//   Children with vertical_policy() == size_policy::fixed get their
//   size_hint().height.  All others share remaining rows weighted by
//   stretch_factor().
//
// spacing() — gap rows between adjacent items (default 0).

class v_box : public widget {
    std::vector<widget *> items_;
    int                   spacing_ = 0;
    margin                margin_  {};

public:
    NX_OBJECT(v_box)

    explicit v_box(nx::core::object * parent = nullptr);

    void set_spacing(int s) noexcept { spacing_ = s; }
    int  spacing()    const noexcept { return spacing_; }

    void set_margin(const nx::tui::margin & m) noexcept { margin_ = m; }
    void set_margin(int all)                   noexcept { margin_ = nx::tui::margin{all}; }
    [[nodiscard]] const nx::tui::margin & margin() const noexcept { return margin_; }

    // Append w (reparenting if necessary).
    void add(widget * w);

    // Insert w at position idx (0-based).
    void insert(int idx, widget * w);

    // Insert w immediately before `before`; appends if `before` is not found.
    void insert_before(widget * before, widget * w);

    // Remove w from layout and reparent to nullptr.
    void remove(widget * w);

    const std::vector<widget *> & items() const noexcept { return items_; }

    [[nodiscard]] size_type minimum_size() const override;

protected:
    void _apply_layout() override;
};

// ── h_box ─────────────────────────────────────────────────────────────────────
//
// Container widget that arranges its children horizontally.
// Same item-management API as v_box; sizes children by horizontal_policy() and
// stretch_factor().
//
// spacing() — gap columns between adjacent items (default 0).

class h_box : public widget {
    std::vector<widget *> items_;
    int                   spacing_ = 0;
    margin                margin_  {};

public:
    NX_OBJECT(h_box)

    explicit h_box(nx::core::object * parent = nullptr);

    void set_spacing(int s) noexcept { spacing_ = s; }
    int  spacing()    const noexcept { return spacing_; }

    void set_margin(const nx::tui::margin & m) noexcept { margin_ = m; }
    void set_margin(int all)                   noexcept { margin_ = nx::tui::margin{all}; }
    [[nodiscard]] const nx::tui::margin & margin() const noexcept { return margin_; }

    void add(widget * w);
    void insert(int idx, widget * w);
    void insert_before(widget * before, widget * w);
    void remove(widget * w);

    const std::vector<widget *> & items() const noexcept { return items_; }

    [[nodiscard]] size_type minimum_size() const override;

protected:
    void _apply_layout() override;
};

} // namespace nx::tui
