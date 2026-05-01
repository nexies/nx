#pragma once

#include <nx/tui/layout/layout.hpp>

namespace nx::tui {

// ── stack_alignment ───────────────────────────────────────────────────────────

enum class stack_alignment : uint8_t {
    start,   // children sit at the top  (v_stack) / left  (h_stack)
    center,  // children sit in the middle
    end,     // children sit at the bottom (v_stack) / right (h_stack)
};

// ── v_stack ───────────────────────────────────────────────────────────────────
//
// Stacks children vertically.  Unlike v_box, children are never expanded along
// the main axis — each always gets its size_hint().height.  The group as a
// whole is positioned within the available space according to `alignment`.
//
// Cross axis (width): children fill the full available width, same as v_box.

class v_stack : public layout {
    stack_alignment align_ = stack_alignment::start;

public:
    NX_OBJECT(v_stack)

    explicit v_stack(nx::core::object * parent = nullptr);

    void set_alignment(stack_alignment a) noexcept { align_ = a; }
    [[nodiscard]] stack_alignment alignment() const noexcept { return align_; }

    [[nodiscard]] size_type minimum_size() const override;

protected:
    void _apply_layout() override;
};

// ── h_stack ───────────────────────────────────────────────────────────────────
//
// Arranges children horizontally without expanding them — each gets its
// size_hint().width.  The group is positioned according to `alignment`.
//
// Cross axis (height): children fill the full available height.

class h_stack : public layout {
    stack_alignment align_ = stack_alignment::start;

public:
    NX_OBJECT(h_stack)

    explicit h_stack(nx::core::object * parent = nullptr);

    void set_alignment(stack_alignment a) noexcept { align_ = a; }
    [[nodiscard]] stack_alignment alignment() const noexcept { return align_; }

    [[nodiscard]] size_type minimum_size() const override;

protected:
    void _apply_layout() override;
};

// ── a_box ─────────────────────────────────────────────────────────────────────
//
// Overlay container: all children are given the same rect (the parent's inner
// area after margins).  Children are painted in insertion order — first child
// is drawn first (bottom), last child is drawn on top.
//
// Useful for layered UIs: background + foreground, modal overlays, badge
// decorations painted over a base widget.

class a_box : public layout {
public:
    NX_OBJECT(a_box)

    explicit a_box(nx::core::object * parent = nullptr);

    [[nodiscard]] size_type minimum_size() const override;

protected:
    void _apply_layout() override;
};

} // namespace nx::tui
