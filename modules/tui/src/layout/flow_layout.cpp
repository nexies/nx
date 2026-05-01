#include <nx/tui/layout/flow.hpp>

#include <algorithm>

namespace nx::tui {

h_flow::h_flow(nx::core::object * parent)
    : layout(parent)
{}

widget::size_type h_flow::minimum_size() const
{
    // Worst case: every child on its own row (container as narrow as widest child).
    int min_w = 0, min_h = 0, n = 0;
    for (auto * c : child_widgets()) {
        if (!c->is_visible()) continue;
        const auto sh = c->size_hint();
        min_w  = std::max(min_w, sh.width);
        min_h += sh.height;
        ++n;
    }
    const int rs = row_spacing_ >= 0 ? row_spacing_ : spacing_;
    min_h += rs * std::max(0, n - 1);
    min_h += margin_.top  + margin_.bottom;
    min_w += margin_.left + margin_.right;
    return { min_h, min_w };
}

void h_flow::_apply_layout()
{
    _sync_items();

    const int pw = size().width;
    const int ph = size().height;

    const int ml = std::max(0, margin_.left);
    const int mr = std::max(0, margin_.right);
    const int mt = std::max(0, margin_.top);
    const int mb = std::max(0, margin_.bottom);

    const int inner_x = ml;
    const int inner_w = std::max(0, pw - ml - mr);
    (void)ph; (void)mb; // height grows naturally; no forced clamp

    const int h_gap = spacing_;
    const int v_gap = row_spacing_ >= 0 ? row_spacing_ : spacing_;

    int cur_x   = inner_x;
    int cur_y   = mt;
    int row_h   = 0;
    bool first_in_row = true;

    for (auto * c : items_) {
        if (!c->is_visible()) continue;

        const int cw = std::max(0, c->size_hint().width);
        const int ch = std::max(0, c->size_hint().height);

        // Wrap if item doesn't fit, unless it's the first item on this row
        // (always place at least one item per row to avoid infinite loops).
        if (!first_in_row && cur_x + cw > inner_x + inner_w) {
            cur_y      += row_h + v_gap;
            cur_x       = inner_x;
            row_h       = 0;
            first_in_row = true;
        }

        c->set_geometry({cur_x, cur_y, cw, ch});

        cur_x       += cw + h_gap;
        row_h        = std::max(row_h, ch);
        first_in_row = false;
    }
}

} // namespace nx::tui
