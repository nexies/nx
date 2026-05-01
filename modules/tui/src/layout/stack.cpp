#include <nx/tui/layout/stack.hpp>

#include <algorithm>
#include <vector>

namespace nx::tui {

namespace {

std::vector<widget *> visible_items(const std::vector<widget *> & items)
{
    std::vector<widget *> out;
    out.reserve(items.size());
    for (auto * w : items)
        if (w->is_visible()) out.push_back(w);
    return out;
}

} // namespace

// ── v_stack ───────────────────────────────────────────────────────────────────

v_stack::v_stack(nx::core::object * parent)
    : layout(parent)
{}

widget::size_type v_stack::minimum_size() const
{
    int min_h = 0, min_w = 0, n = 0;
    for (auto * c : child_widgets()) {
        if (!c->is_visible()) continue;
        const auto sh = c->size_hint();
        min_h += sh.height;
        min_w  = std::max(min_w, sh.width);
        ++n;
    }
    min_h += spacing_ * std::max(0, n - 1);
    min_h += margin_.top  + margin_.bottom;
    min_w += margin_.left + margin_.right;
    return { min_h, min_w };
}

void v_stack::_apply_layout()
{
    _sync_items();
    auto children = visible_items(items_);
    if (children.empty()) return;

    const int pw = size().width;
    const int ph = size().height;
    const int n  = static_cast<int>(children.size());

    const int ml = std::max(0, margin_.left);
    const int mr = std::max(0, margin_.right);
    const int mt = std::max(0, margin_.top);
    const int mb = std::max(0, margin_.bottom);

    const int inner_x = ml;
    const int inner_w = std::max(0, pw - ml - mr);
    const int inner_h = std::max(0, ph - mt - mb);

    // Total height consumed by children and their gaps.
    int total_h = spacing_ * std::max(0, n - 1);
    for (auto * c : children)
        total_h += std::max(0, c->size_hint().height);

    // Starting y determined by alignment.
    int y = mt;
    switch (align_) {
    case stack_alignment::start:
        y = mt;
        break;
    case stack_alignment::center:
        y = mt + std::max(0, inner_h - total_h) / 2;
        break;
    case stack_alignment::end:
        y = mt + std::max(0, inner_h - total_h);
        break;
    }

    for (int i = 0; i < n; ++i) {
        auto * c = children[i];
        const int h = std::max(0, c->size_hint().height);
        c->set_geometry({inner_x, y, inner_w, h});
        y += h;
        if (i < n - 1) y += spacing_;
    }
}

// ── h_stack ───────────────────────────────────────────────────────────────────

h_stack::h_stack(nx::core::object * parent)
    : layout(parent)
{}

widget::size_type h_stack::minimum_size() const
{
    int min_h = 0, min_w = 0, n = 0;
    for (auto * c : child_widgets()) {
        if (!c->is_visible()) continue;
        const auto sh = c->size_hint();
        min_w += sh.width;
        min_h  = std::max(min_h, sh.height);
        ++n;
    }
    min_w += spacing_ * std::max(0, n - 1);
    min_h += margin_.top  + margin_.bottom;
    min_w += margin_.left + margin_.right;
    return { min_h, min_w };
}

void h_stack::_apply_layout()
{
    _sync_items();
    auto children = visible_items(items_);
    if (children.empty()) return;

    const int pw = size().width;
    const int ph = size().height;
    const int n  = static_cast<int>(children.size());

    const int ml = std::max(0, margin_.left);
    const int mr = std::max(0, margin_.right);
    const int mt = std::max(0, margin_.top);
    const int mb = std::max(0, margin_.bottom);

    const int inner_y = mt;
    const int inner_w = std::max(0, pw - ml - mr);
    const int inner_h = std::max(0, ph - mt - mb);

    int total_w = spacing_ * std::max(0, n - 1);
    for (auto * c : children)
        total_w += std::max(0, c->size_hint().width);

    int x = ml;
    switch (align_) {
    case stack_alignment::start:
        x = ml;
        break;
    case stack_alignment::center:
        x = ml + std::max(0, inner_w - total_w) / 2;
        break;
    case stack_alignment::end:
        x = ml + std::max(0, inner_w - total_w);
        break;
    }

    for (int i = 0; i < n; ++i) {
        auto * c = children[i];
        const int w = std::max(0, c->size_hint().width);
        c->set_geometry({x, inner_y, w, inner_h});
        x += w;
        if (i < n - 1) x += spacing_;
    }
}

// ── a_box ─────────────────────────────────────────────────────────────────────

a_box::a_box(nx::core::object * parent)
    : layout(parent)
{}

widget::size_type a_box::minimum_size() const
{
    // Minimum is the largest minimum among all children (they all overlap).
    int min_h = 0, min_w = 0;
    for (auto * c : child_widgets()) {
        if (!c->is_visible()) continue;
        const auto ms = c->minimum_size();
        min_h = std::max(min_h, ms.height);
        min_w = std::max(min_w, ms.width);
    }
    min_h += margin_.top  + margin_.bottom;
    min_w += margin_.left + margin_.right;
    return { min_h, min_w };
}

void a_box::_apply_layout()
{
    _sync_items();

    const int pw = size().width;
    const int ph = size().height;

    const int ml = std::max(0, margin_.left);
    const int mr = std::max(0, margin_.right);
    const int mt = std::max(0, margin_.top);
    const int mb = std::max(0, margin_.bottom);

    const int inner_x = ml;
    const int inner_y = mt;
    const int inner_w = std::max(0, pw - ml - mr);
    const int inner_h = std::max(0, ph - mt - mb);

    for (auto * c : items_)
        c->set_geometry({inner_x, inner_y, inner_w, inner_h});
}

} // namespace nx::tui
