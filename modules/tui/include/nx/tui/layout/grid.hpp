#pragma once

#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/types/margin.hpp>

#include <vector>

namespace nx::tui {

// ── grid_layout ───────────────────────────────────────────────────────────────
//
// Arranges children in a regular M×N grid.
//
// Each widget is placed in a specific (row, col) cell via place().  Cells are
// sized as follows:
//   - Column width  = max size_hint().width  of all widgets in that column.
//   - Row height    = max size_hint().height of all widgets in that row.
//   - Columns/rows with stretch > 0 share the remaining space proportionally.
//     Columns/rows with stretch == 0 are fixed (sized to content).
//
// set_h_spacing() — horizontal gap between columns.
// set_v_spacing() — vertical gap between rows.
// set_margin()    — outer inset applied before placement.

class grid : public widget {
public:
    NX_OBJECT(grid)

    explicit grid(nx::core::object * parent = nullptr);

    // Place w at (row, col).  Reparents w to this grid if needed.
    // Replaces any existing widget at that cell.
    void place(widget * w, int row, int col);

    // Remove w from the grid and reparent to nullptr.
    void remove(widget * w);

    // Spacing between cells.
    void set_h_spacing(int s) noexcept { h_spacing_ = s; }
    void set_v_spacing(int s) noexcept { v_spacing_ = s; }
    void set_spacing  (int s) noexcept { h_spacing_ = v_spacing_ = s; }
    [[nodiscard]] int h_spacing() const noexcept { return h_spacing_; }
    [[nodiscard]] int v_spacing() const noexcept { return v_spacing_; }

    void set_margin(const nx::tui::margin & m) noexcept { margin_ = m; }
    void set_margin(int all)                   noexcept { margin_ = nx::tui::margin{all}; }
    [[nodiscard]] const nx::tui::margin & margin() const noexcept { return margin_; }

    // Stretch factor for a column/row.
    //   0 (default) — fixed: sized to the widest/tallest widget in that column/row.
    //   >= 1        — expands: shares remaining space proportionally by weight.
    void set_column_stretch(int col, int stretch);
    void set_row_stretch   (int row, int stretch);

    [[nodiscard]] size_type minimum_size() const override;

private:
    struct cell {
        widget * w   = nullptr;
        int      row = 0;
        int      col = 0;
    };

    std::vector<cell>   cells_;
    int                 h_spacing_ = 0;
    int                 v_spacing_ = 0;
    nx::tui::margin     margin_    {};
    std::vector<int>    col_stretches_;
    std::vector<int>    row_stretches_;

    [[nodiscard]] int _col_stretch(int col) const noexcept;
    [[nodiscard]] int _row_stretch(int row) const noexcept;

    // Compute per-column widths and per-row heights from current children,
    // distributing remaining space according to stretch factors.
    void _compute_sizes(int avail_w, int avail_h,
                        std::vector<int> & col_w,
                        std::vector<int> & row_h) const;

protected:
    void _apply_layout() override;
};

} // namespace nx::tui
