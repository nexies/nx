#pragma once

#include <nx/tui/layout/layout.hpp>

namespace nx::tui {

// ── v_box ─────────────────────────────────────────────────────────────────────
//
// Stacks children vertically, distributing available height among them.
//
// Children with vertical_policy() == size_policy::fixed get their
// size_hint().height; all others share the remaining rows weighted by
// stretch_factor().

class v_box : public layout {
public:
    NX_OBJECT(v_box)

    explicit v_box(nx::core::object * parent = nullptr);

    [[nodiscard]] size_type minimum_size() const override;

protected:
    void _apply_layout() override;
};

// ── h_box ─────────────────────────────────────────────────────────────────────
//
// Arranges children horizontally, distributing available width among them.
//
// Children with horizontal_policy() == size_policy::fixed get their
// size_hint().width; all others share the remaining columns weighted by
// stretch_factor().

class h_box : public layout {
public:
    NX_OBJECT(h_box)

    explicit h_box(nx::core::object * parent = nullptr);

    [[nodiscard]] size_type minimum_size() const override;

protected:
    void _apply_layout() override;
};

} // namespace nx::tui
