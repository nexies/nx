#include <nx/tui/layout/layout.hpp>

#include <algorithm>

namespace nx::tui {

layout::layout(nx::core::object * parent)
    : widget(parent)
{}

void layout::_sync_items()
{
    auto all = child_widgets();

    // Remove stale entries.
    items_.erase(
        std::remove_if(items_.begin(), items_.end(), [&](widget * w) {
            return std::find(all.begin(), all.end(), w) == all.end();
        }),
        items_.end()
    );

    // Append new children (e.g. added implicitly via their constructor).
    for (auto * w : all)
        if (std::find(items_.begin(), items_.end(), w) == items_.end())
            items_.push_back(w);
}

void layout::add(widget * w)
{
    if (!w) return;
    w->set_parent(this);
    _sync_items();
}

void layout::insert(int idx, widget * w)
{
    if (!w) return;
    w->set_parent(this);
    _sync_items();
    // Move the last element (just appended by _sync_items) to idx.
    if (items_.empty()) return;
    const int last = static_cast<int>(items_.size()) - 1;
    idx = std::max(0, std::min(idx, last));
    if (idx != last) {
        widget * tmp = items_[last];
        items_.erase(items_.begin() + last);
        items_.insert(items_.begin() + idx, tmp);
    }
}

void layout::insert_before(widget * before, widget * w)
{
    if (!w) return;
    w->set_parent(this);
    _sync_items();
    auto it = std::find(items_.begin(), items_.end(), before);
    if (it == items_.end()) return;
    const int idx  = static_cast<int>(it - items_.begin());
    const int last = static_cast<int>(items_.size()) - 1;
    if (idx != last) {
        widget * tmp = items_[last];
        items_.erase(items_.begin() + last);
        items_.insert(items_.begin() + idx, tmp);
    }
}

void layout::remove(widget * w)
{
    if (!w) return;
    w->set_parent(nullptr);
    items_.erase(std::remove(items_.begin(), items_.end(), w), items_.end());
}

} // namespace nx::tui
