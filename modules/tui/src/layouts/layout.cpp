#include <nx/tui/layouts/layout.hpp>
#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/types/size_policy.hpp>

#include <algorithm>
#include <vector>

namespace nx::tui {

// ── helpers ───────────────────────────────────────────────────────────────────

namespace {

// Collect visible direct children.
std::vector<widget *> visible_children(widget & parent)
{
    std::vector<widget *> out;
    for (auto * c : parent.child_widgets())
        if (c->is_visible()) out.push_back(c);
    return out;
}

} // namespace

// ── v_box_layout ──────────────────────────────────────────────────────────────

void v_box_layout::apply(widget & parent)
{
    auto children = visible_children(parent);
    if (children.empty()) return;

    const int parent_w = parent.size().width;
    const int parent_h = parent.size().height;
    const int n        = static_cast<int>(children.size());

    // Spacing between adjacent children eats into the total height.
    const int total_spacing = spacing_ * std::max(0, n - 1);

    // Pass 1: collect fixed heights and total stretch weight.
    int fixed_h       = 0;
    int total_stretch = 0;
    for (auto * c : children) {
        if (c->vertical_policy() == size_policy::fixed) {
            fixed_h += c->size_hint().height;
        } else {
            total_stretch += std::max(1, c->stretch_factor());
        }
    }

    const int remaining = std::max(0, parent_h - fixed_h - total_spacing);
    if (total_stretch == 0) total_stretch = 1; // guard against div-by-zero

    // Pass 2: assign geometries.
    int y            = 0;
    int flex_idx     = 0;    // how many flexible children we have processed
    int flex_count   = 0;    // total number of flexible children
    int flex_given   = 0;    // height already assigned to flexible children

    for (auto * c : children)
        if (c->vertical_policy() != size_policy::fixed) ++flex_count;

    for (int i = 0; i < n; ++i) {
        auto * c = children[i];
        int h;

        if (c->vertical_policy() == size_policy::fixed) {
            h = std::max(0, c->size_hint().height);
        } else {
            ++flex_idx;
            if (flex_idx == flex_count) {
                // Last flexible child absorbs any rounding remainder.
                h = remaining - flex_given;
            } else {
                h = (remaining * std::max(1, c->stretch_factor())) / total_stretch;
                flex_given += h;
            }
            h = std::max(0, h);
        }

        c->set_geometry({ 0, y, parent_w, h });
        y += h;
        if (i < n - 1) y += spacing_;
    }
}

// ── h_box_layout ──────────────────────────────────────────────────────────────

void h_box_layout::apply(widget & parent)
{
    auto children = visible_children(parent);
    if (children.empty()) return;

    const int parent_w = parent.size().width;
    const int parent_h = parent.size().height;
    const int n        = static_cast<int>(children.size());

    const int total_spacing = spacing_ * std::max(0, n - 1);

    // Pass 1: fixed widths and total stretch.
    int fixed_w       = 0;
    int total_stretch = 0;
    for (auto * c : children) {
        if (c->horizontal_policy() == size_policy::fixed) {
            fixed_w += c->size_hint().width;
        } else {
            total_stretch += std::max(1, c->stretch_factor());
        }
    }

    const int remaining = std::max(0, parent_w - fixed_w - total_spacing);
    if (total_stretch == 0) total_stretch = 1;

    // Pass 2: assign geometries.
    int x          = 0;
    int flex_idx   = 0;
    int flex_count = 0;
    int flex_given = 0;

    for (auto * c : children)
        if (c->horizontal_policy() != size_policy::fixed) ++flex_count;

    for (int i = 0; i < n; ++i) {
        auto * c = children[i];
        int w;

        if (c->horizontal_policy() == size_policy::fixed) {
            w = std::max(0, c->size_hint().width);
        } else {
            ++flex_idx;
            if (flex_idx == flex_count) {
                w = remaining - flex_given;
            } else {
                w = (remaining * std::max(1, c->stretch_factor())) / total_stretch;
                flex_given += w;
            }
            w = std::max(0, w);
        }

        c->set_geometry({ x, 0, w, parent_h });
        x += w;
        if (i < n - 1) x += spacing_;
    }
}

} // namespace nx::tui
