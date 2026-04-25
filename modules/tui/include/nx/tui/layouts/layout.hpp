#pragma once

namespace nx::tui {

class widget;

// ── layout ────────────────────────────────────────────────────────────────────
//
// Base class for widget layout managers.
// Implement apply(parent) to reposition child widgets inside parent's geometry.
// Called by screen::render() at the start of each render pass.

class layout {
public:
    virtual ~layout() = default;
    virtual void apply(widget & parent) = 0;
};

// ── v_box_layout ──────────────────────────────────────────────────────────────
//
// Stacks visible children vertically.
//
// Children with vertical_policy() == size_policy::fixed get exactly
// size_hint().height rows.  All others share the remaining rows weighted by
// their stretch_factor() (default 1).
//
// spacing()  — gap rows inserted between adjacent children (default 0).

class v_box_layout : public layout {
    int spacing_ = 0;
public:
    void set_spacing(int s) noexcept { spacing_ = s; }
    int  spacing()    const noexcept { return spacing_; }
    void apply(widget & parent) override;
};

// ── h_box_layout ──────────────────────────────────────────────────────────────
//
// Arranges visible children horizontally.
//
// Children with horizontal_policy() == size_policy::fixed get exactly
// size_hint().width columns.  All others share the remaining columns weighted
// by their stretch_factor() (default 1).
//
// spacing()  — gap columns inserted between adjacent children (default 0).

class h_box_layout : public layout {
    int spacing_ = 0;
public:
    void set_spacing(int s) noexcept { spacing_ = s; }
    int  spacing()    const noexcept { return spacing_; }
    void apply(widget & parent) override;
};

} // namespace nx::tui
