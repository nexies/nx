#pragma once

namespace nx::tui {
    // ── margin ────────────────────────────────────────────────────────────────────
    //
    // Inset from each edge of a container before child layout begins.
    // All values are in character cells; negative values are clamped to 0.
    //
    // Convenience constructors:
    //   margin {}                 — all zero
    //   margin { 1 }              — uniform: all sides = 1
    //   margin { 1, 2 }           — vertical=1 (top+bottom), horizontal=2 (left+right)
    //   margin { top, right, bottom, left }   — per-side (CSS order)

    struct margin {
        int top = 0;
        int right = 0;
        int bottom = 0;
        int left = 0;

        // ── Constructors ──────────────────────────────────────────────────────────

        constexpr
        margin() noexcept = default;

        // Uniform — all four sides equal.
        explicit constexpr
        margin(int all) noexcept
            : top(all), right(all), bottom(all), left(all) {
        }

        // Axis-based — vertical (top/bottom) and horizontal (left/right).
        constexpr
        margin(int vertical, int horizontal) noexcept
            : top(vertical), right(horizontal), bottom(vertical), left(horizontal) {
        }

        // Per-side (CSS order: top, right, bottom, left).
        constexpr
        margin(int t, int r, int b, int l) noexcept
            : top(t), right(r), bottom(b), left(l) {
        }

        // ── Queries ───────────────────────────────────────────────────────────────

        NX_NODISCARD constexpr int
        horizontal() const noexcept {
            return left + right;
        }

        NX_NODISCARD constexpr int
        vertical() const noexcept {
            return top + bottom;
        }

        NX_NODISCARD constexpr bool
        is_zero() const noexcept {
            return top == 0 && right == 0 && bottom == 0 && left == 0;
        }

        NX_NODISCARD constexpr bool
        operator==(const margin &o) const noexcept {
            return top == o.top && right == o.right && bottom == o.bottom && left == o.left;
        }

        NX_NODISCARD constexpr bool
        operator!=(const margin &o) const noexcept {
            return !(*this == o);
        }
    };
} // namespace nx::tui
