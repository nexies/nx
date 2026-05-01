#pragma once

#include <nx/tui/layout/layout.hpp>

namespace nx::tui {

// ── h_flow ───────────────────────────────────────────────────────────────
//
// Places children left-to-right, wrapping to the next row when the current row
// is full.  Each child always gets its size_hint() size — children are never
// stretched or shrunk.
//
// spacing()     — horizontal gap between items on the same row.
// row_spacing() — vertical gap between rows (defaults to spacing()).
// margin()      — insets applied before layout.
//
// minimum_size() returns the "single-column" worst case: widest child ×
// total stacked height.

class h_flow : public layout {
    int row_spacing_ = -1; // -1 means "same as spacing_"

public:
    NX_OBJECT(h_flow)

    explicit h_flow(nx::core::object * parent = nullptr);

    // Vertical gap between rows.  When unset (default), uses spacing().
    void set_row_spacing(int s) noexcept { row_spacing_ = s; }
    [[nodiscard]] int row_spacing() const noexcept
    { return row_spacing_ >= 0 ? row_spacing_ : spacing_; }

    [[nodiscard]] size_type minimum_size() const override;

protected:
    void _apply_layout() override;
};

} // namespace nx::tui
