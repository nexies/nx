#include <nx/tui/widgets/box.hpp>
#include <nx/tui/types/size_policy.hpp>
#include <nx/tui/types/margin.hpp>

#include <algorithm>
#include <vector>

namespace nx::tui {

// ── shared helpers ─────────────────────────────────────────────────────────────

namespace {

// Sync items_ with the current child_widgets() list:
//   - children added implicitly (via constructor) are appended in tree order
//   - children no longer present are removed
//   - children added via add()/insert() keep their explicit position
void sync_items(std::vector<widget *> & items, widget & parent)
{
    auto all = parent.child_widgets();

    // Remove stale entries first.
    items.erase(
        std::remove_if(items.begin(), items.end(), [&](widget * w) {
            return std::find(all.begin(), all.end(), w) == all.end();
        }),
        items.end()
    );

    // Append any new children (e.g. added via implicit constructor).
    for (auto * w : all) {
        if (std::find(items.begin(), items.end(), w) == items.end())
            items.push_back(w);
    }
}

// Build visible-only sublist for layout passes.
std::vector<widget *> visible_items(const std::vector<widget *> & items)
{
    std::vector<widget *> out;
    out.reserve(items.size());
    for (auto * w : items)
        if (w->is_visible()) out.push_back(w);
    return out;
}

// Move the last element of v to index idx (used after a reparent appended it).
void move_to_index(std::vector<widget *> & v, int idx)
{
    if (v.empty()) return;
    const int last = static_cast<int>(v.size()) - 1;
    idx = std::max(0, std::min(idx, last));
    if (idx == last) return;
    widget * w = v[last];
    v.erase(v.begin() + last);
    v.insert(v.begin() + idx, w);
}

// Clamp lo+hi so that (avail - lo - hi) >= min_content.
// Reduction is distributed proportionally between lo and hi.
void clamp_margin_axis(int avail, int min_content, int & lo, int & hi)
{
    lo = std::max(0, lo);
    hi = std::max(0, hi);
    const int budget = std::max(0, avail - min_content);
    const int sum    = lo + hi;
    if (sum <= budget) return;
    if (sum == 0) return;   // guard div-by-zero
    lo = (lo * budget) / sum;
    hi = budget - lo;
}

// Vertical layout algorithm.
void apply_vbox(widget & parent, std::vector<widget *> & items,
                int spacing, const margin & m)
{
    auto children = visible_items(items);
    if (children.empty()) return;

    const int pw = parent.size().width;
    const int ph = parent.size().height;
    const int n  = static_cast<int>(children.size());

    const int total_spacing = spacing * std::max(0, n - 1);

    int fixed_h       = 0;
    int total_stretch = 0;
    for (auto * c : children) {
        if (c->vertical_policy() == size_policy::fixed)
            fixed_h += c->size_hint().height;
        else
            total_stretch += std::max(1, c->stretch_factor());
    }

    // Primary axis: fixed children + gaps.
    const int min_content_h = fixed_h + total_spacing;

    // Cross axis: children fill the full inner_w, so the limiting factor is
    // the widest child's minimum width.
    int min_cross_w = 0;
    for (auto * c : children)
        min_cross_w = std::max(min_cross_w, c->minimum_size().width);

    int ml = m.left,  mr = m.right;
    int mt = m.top,   mb = m.bottom;
    clamp_margin_axis(pw, min_cross_w,  ml, mr);
    clamp_margin_axis(ph, min_content_h, mt, mb);

    const int inner_x = ml;
    const int inner_y = mt;
    const int inner_w = pw - ml - mr;
    const int inner_h = ph - mt - mb;

    const int remaining = std::max(0, inner_h - min_content_h);
    if (total_stretch == 0) total_stretch = 1;

    int y          = inner_y;
    int flex_idx   = 0;
    int flex_count = 0;
    int flex_given = 0;

    for (auto * c : children)
        if (c->vertical_policy() != size_policy::fixed) ++flex_count;

    for (int i = 0; i < n; ++i) {
        auto * c = children[i];
        int h;

        if (c->vertical_policy() == size_policy::fixed) {
            h = std::max(0, c->size_hint().height);
        } else {
            ++flex_idx;
            if (flex_idx == flex_count)
                h = remaining - flex_given;
            else {
                h = (remaining * std::max(1, c->stretch_factor())) / total_stretch;
                flex_given += h;
            }
            h = std::max(0, h);
        }

        c->set_geometry({inner_x, y, inner_w, h});
        y += h;
        if (i < n - 1) y += spacing;
    }
}

// Horizontal layout algorithm.
void apply_hbox(widget & parent, std::vector<widget *> & items,
                int spacing, const margin & m)
{
    auto children = visible_items(items);
    if (children.empty()) return;

    const int pw = parent.size().width;
    const int ph = parent.size().height;
    const int n  = static_cast<int>(children.size());

    const int total_spacing = spacing * std::max(0, n - 1);

    int fixed_w       = 0;
    int total_stretch = 0;
    for (auto * c : children) {
        if (c->horizontal_policy() == size_policy::fixed)
            fixed_w += c->size_hint().width;
        else
            total_stretch += std::max(1, c->stretch_factor());
    }

    const int min_content_w = fixed_w + total_spacing;

    // Cross axis: all children get inner_h; worst-case minimum height wins.
    int min_cross_h = 0;
    for (auto * c : children)
        min_cross_h = std::max(min_cross_h, c->minimum_size().height);

    int ml = m.left,  mr = m.right;
    int mt = m.top,   mb = m.bottom;
    clamp_margin_axis(pw, min_content_w, ml, mr);
    clamp_margin_axis(ph, min_cross_h,   mt, mb);

    const int inner_x = ml;
    const int inner_y = mt;
    const int inner_w = pw - ml - mr;
    const int inner_h = ph - mt - mb;

    const int remaining = std::max(0, inner_w - min_content_w);
    if (total_stretch == 0) total_stretch = 1;

    int x          = inner_x;
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
            if (flex_idx == flex_count)
                w = remaining - flex_given;
            else {
                w = (remaining * std::max(1, c->stretch_factor())) / total_stretch;
                flex_given += w;
            }
            w = std::max(0, w);
        }

        c->set_geometry({x, inner_y, w, inner_h});
        x += w;
        if (i < n - 1) x += spacing;
    }
}

} // namespace

// ── v_box ─────────────────────────────────────────────────────────────────────

v_box::v_box(nx::core::object * parent)
    : widget(parent)
{}

void v_box::add(widget * w)
{
    if (!w) return;
    w->set_parent(this);
    // Ensure it's in items_ (set_parent may have been called when w was already
    // constructed, so on_child_added's cast succeeds — but sync() catches both).
    sync_items(items_, *this);
}

void v_box::insert(int idx, widget * w)
{
    if (!w) return;
    w->set_parent(this);
    sync_items(items_, *this);
    move_to_index(items_, idx);
}

void v_box::insert_before(widget * before, widget * w)
{
    if (!w) return;
    w->set_parent(this);
    sync_items(items_, *this);
    auto it = std::find(items_.begin(), items_.end(), before);
    if (it != items_.end())
        move_to_index(items_, static_cast<int>(it - items_.begin()));
}

void v_box::remove(widget * w)
{
    if (!w) return;
    w->set_parent(nullptr);
    items_.erase(std::remove(items_.begin(), items_.end(), w), items_.end());
}

widget::size_type v_box::minimum_size() const
{
    int min_h = 0, min_w = 0, n = 0;
    for (auto * c : child_widgets()) {
        if (!c->is_visible()) continue;
        auto ms = c->minimum_size();
        min_h += ms.height;
        min_w  = std::max(min_w, ms.width);
        ++n;
    }
    min_h += spacing_ * std::max(0, n - 1);
    // Include margin in the box's own minimum size.
    min_h += margin_.top  + margin_.bottom;
    min_w += margin_.left + margin_.right;
    return { min_h, min_w };
}

void v_box::_apply_layout()
{
    sync_items(items_, *this);
    apply_vbox(*this, items_, spacing_, margin_);
}

// ── h_box ─────────────────────────────────────────────────────────────────────

h_box::h_box(nx::core::object * parent)
    : widget(parent)
{}

void h_box::add(widget * w)
{
    if (!w) return;
    w->set_parent(this);
    sync_items(items_, *this);
}

void h_box::insert(int idx, widget * w)
{
    if (!w) return;
    w->set_parent(this);
    sync_items(items_, *this);
    move_to_index(items_, idx);
}

void h_box::insert_before(widget * before, widget * w)
{
    if (!w) return;
    w->set_parent(this);
    sync_items(items_, *this);
    auto it = std::find(items_.begin(), items_.end(), before);
    if (it != items_.end())
        move_to_index(items_, static_cast<int>(it - items_.begin()));
}

void h_box::remove(widget * w)
{
    if (!w) return;
    w->set_parent(nullptr);
    items_.erase(std::remove(items_.begin(), items_.end(), w), items_.end());
}

widget::size_type h_box::minimum_size() const
{
    int min_h = 0, min_w = 0, n = 0;
    for (auto * c : child_widgets()) {
        if (!c->is_visible()) continue;
        auto ms = c->minimum_size();
        min_w += ms.width;
        min_h  = std::max(min_h, ms.height);
        ++n;
    }
    min_w += spacing_ * std::max(0, n - 1);
    min_h += margin_.top  + margin_.bottom;
    min_w += margin_.left + margin_.right;
    return { min_h, min_w };
}

void h_box::_apply_layout()
{
    sync_items(items_, *this);
    apply_hbox(*this, items_, spacing_, margin_);
}

} // namespace nx::tui
