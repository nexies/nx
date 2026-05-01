#include <nx/tui/layout/grid.hpp>

#include <algorithm>
#include <vector>

namespace nx::tui {

grid::grid(nx::core::object * parent)
    : widget(parent)
{}

// ── cell management ───────────────────────────────────────────────────────────

void grid::place(widget * w, int row, int col)
{
    if (!w || row < 0 || col < 0) return;

    // Remove any existing widget at this cell.
    for (auto & c : cells_) {
        if (c.row == row && c.col == col && c.w && c.w != w) {
            c.w->set_parent(nullptr);
            c.w = nullptr;
        }
    }
    // Remove w from any previous cell.
    cells_.erase(std::remove_if(cells_.begin(), cells_.end(),
        [&](const cell & c) {
            return (c.w == w) || (c.w == nullptr && c.row == row && c.col == col);
        }), cells_.end());

    w->set_parent(this);
    cells_.push_back({w, row, col});
}

void grid::remove(widget * w)
{
    if (!w) return;
    for (auto & c : cells_)
        if (c.w == w) { c.w = nullptr; break; }
    cells_.erase(std::remove_if(cells_.begin(), cells_.end(),
        [](const cell & c) { return c.w == nullptr; }), cells_.end());
    w->set_parent(nullptr);
}

// ── stretch ───────────────────────────────────────────────────────────────────

void grid::set_column_stretch(int col, int stretch)
{
    if (col < 0) return;
    if (col >= static_cast<int>(col_stretches_.size()))
        col_stretches_.resize(col + 1, 0);
    col_stretches_[col] = std::max(0, stretch);
}

void grid::set_row_stretch(int row, int stretch)
{
    if (row < 0) return;
    if (row >= static_cast<int>(row_stretches_.size()))
        row_stretches_.resize(row + 1, 0);
    row_stretches_[row] = std::max(0, stretch);
}

int grid::_col_stretch(int col) const noexcept
{
    if (col >= 0 && col < static_cast<int>(col_stretches_.size()))
        return col_stretches_[col];
    return 0;
}

int grid::_row_stretch(int row) const noexcept
{
    if (row >= 0 && row < static_cast<int>(row_stretches_.size()))
        return row_stretches_[row];
    return 0;
}

// ── size computation ──────────────────────────────────────────────────────────

void grid::_compute_sizes(int avail_w, int avail_h,
                                  std::vector<int> & col_w,
                                  std::vector<int> & row_h) const
{
    if (cells_.empty()) return;

    int num_cols = 0, num_rows = 0;
    for (auto & c : cells_) {
        if (c.w && c.w->is_visible()) {
            num_cols = std::max(num_cols, c.col + 1);
            num_rows = std::max(num_rows, c.row + 1);
        }
    }
    if (num_cols == 0 || num_rows == 0) return;

    col_w.assign(num_cols, 0);
    row_h.assign(num_rows, 0);

    // Content-based natural sizes.
    for (auto & c : cells_) {
        if (!c.w || !c.w->is_visible()) continue;
        const auto sh = c.w->size_hint();
        col_w[c.col] = std::max(col_w[c.col], sh.width);
        row_h[c.row] = std::max(row_h[c.row], sh.height);
    }

    // Distribute remaining width to stretching columns.
    {
        int fixed_w = h_spacing_ * std::max(0, num_cols - 1);
        int total_stretch = 0;
        for (int c = 0; c < num_cols; ++c) {
            if (_col_stretch(c) == 0) fixed_w += col_w[c];
            else total_stretch += _col_stretch(c);
        }
        if (total_stretch > 0) {
            const int remaining = std::max(0, avail_w - fixed_w);
            int given = 0, flex_idx = 0, flex_count = 0;
            for (int c = 0; c < num_cols; ++c)
                if (_col_stretch(c) > 0) ++flex_count;
            for (int c = 0; c < num_cols; ++c) {
                if (_col_stretch(c) == 0) continue;
                ++flex_idx;
                const int w = (flex_idx == flex_count)
                    ? remaining - given
                    : (remaining * _col_stretch(c)) / total_stretch;
                col_w[c] = std::max(0, w);
                given   += col_w[c];
            }
        }
    }

    // Distribute remaining height to stretching rows.
    {
        int fixed_h = v_spacing_ * std::max(0, num_rows - 1);
        int total_stretch = 0;
        for (int r = 0; r < num_rows; ++r) {
            if (_row_stretch(r) == 0) fixed_h += row_h[r];
            else total_stretch += _row_stretch(r);
        }
        if (total_stretch > 0) {
            const int remaining = std::max(0, avail_h - fixed_h);
            int given = 0, flex_idx = 0, flex_count = 0;
            for (int r = 0; r < num_rows; ++r)
                if (_row_stretch(r) > 0) ++flex_count;
            for (int r = 0; r < num_rows; ++r) {
                if (_row_stretch(r) == 0) continue;
                ++flex_idx;
                const int h = (flex_idx == flex_count)
                    ? remaining - given
                    : (remaining * _row_stretch(r)) / total_stretch;
                row_h[r] = std::max(0, h);
                given   += row_h[r];
            }
        }
    }
}

// ── minimum_size ──────────────────────────────────────────────────────────────

widget::size_type grid::minimum_size() const
{
    // Compute natural sizes (no avail space, so fixed columns only).
    std::vector<int> col_w, row_h;
    _compute_sizes(0, 0, col_w, row_h);

    int min_w = margin_.left + margin_.right;
    int min_h = margin_.top  + margin_.bottom;

    if (!col_w.empty()) {
        const int nc = static_cast<int>(col_w.size());
        const int nr = static_cast<int>(row_h.size());
        for (int w : col_w) min_w += w;
        for (int h : row_h) min_h += h;
        min_w += h_spacing_ * std::max(0, nc - 1);
        min_h += v_spacing_ * std::max(0, nr - 1);
    }
    return { min_h, min_w };
}

// ── _apply_layout ─────────────────────────────────────────────────────────────

void grid::_apply_layout()
{
    if (cells_.empty()) return;

    const int pw = size().width;
    const int ph = size().height;

    const int ml = std::max(0, margin_.left);
    const int mr = std::max(0, margin_.right);
    const int mt = std::max(0, margin_.top);
    const int mb = std::max(0, margin_.bottom);

    const int avail_w = std::max(0, pw - ml - mr);
    const int avail_h = std::max(0, ph - mt - mb);

    std::vector<int> col_w, row_h;
    _compute_sizes(avail_w, avail_h, col_w, row_h);
    if (col_w.empty()) return;

    const int num_cols = static_cast<int>(col_w.size());
    const int num_rows = static_cast<int>(row_h.size());

    // Cumulative column x-positions and row y-positions.
    std::vector<int> col_x(num_cols, ml);
    std::vector<int> row_y(num_rows, mt);
    for (int c = 1; c < num_cols; ++c)
        col_x[c] = col_x[c-1] + col_w[c-1] + h_spacing_;
    for (int r = 1; r < num_rows; ++r)
        row_y[r] = row_y[r-1] + row_h[r-1] + v_spacing_;

    for (auto & c : cells_) {
        if (!c.w || !c.w->is_visible()) continue;
        if (c.col >= num_cols || c.row >= num_rows) continue;
        c.w->set_geometry({col_x[c.col], row_y[c.row],
                           col_w[c.col], row_h[c.row]});
    }
}

} // namespace nx::tui
