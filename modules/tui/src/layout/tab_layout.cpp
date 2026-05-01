#include <nx/tui/layout/tab_layout.hpp>

#include <algorithm>

namespace nx::tui {

tab_layout::tab_layout(nx::core::object * parent)
    : widget(parent)
{}

void tab_layout::add_tab(widget * w)
{
    if (!w) return;
    w->set_parent(this);
    tabs_.push_back(w);

    if (current_ < 0) {
        // First tab: make it current, hide the rest (none yet).
        current_ = 0;
        w->set_visible(true);
    } else {
        // Subsequent tabs: hidden until explicitly selected.
        w->set_visible(false);
    }
    update();
}

void tab_layout::remove_tab(widget * w)
{
    if (!w) return;
    const auto it = std::find(tabs_.begin(), tabs_.end(), w);
    if (it == tabs_.end()) return;

    const int idx = static_cast<int>(it - tabs_.begin());
    tabs_.erase(it);
    w->set_parent(nullptr);

    if (tabs_.empty()) {
        current_ = -1;
    } else if (current_ >= static_cast<int>(tabs_.size())) {
        // Was showing the last tab which was removed — go to new last.
        set_current(static_cast<int>(tabs_.size()) - 1);
        return; // set_current calls update() and emits signal
    } else if (current_ == idx) {
        // Removed the current tab — re-show the tab now at this index.
        set_current(current_);
        return;
    }
    update();
}

void tab_layout::set_current(int idx)
{
    if (tabs_.empty()) { current_ = -1; return; }
    idx = std::max(0, std::min(idx, static_cast<int>(tabs_.size()) - 1));
    if (idx == current_) return;

    if (current_ >= 0 && current_ < static_cast<int>(tabs_.size()))
        tabs_[current_]->set_visible(false);

    current_ = idx;
    tabs_[current_]->set_visible(true);

    update();
    NX_EMIT(current_changed, current_)
}

widget::size_type tab_layout::minimum_size() const
{
    // The minimum must accommodate the largest tab.
    int min_h = 0, min_w = 0;
    for (auto * t : tabs_) {
        const auto ms = t->minimum_size();
        min_h = std::max(min_h, ms.height);
        min_w = std::max(min_w, ms.width);
    }
    return { min_h, min_w };
}

void tab_layout::_apply_layout()
{
    // All tabs share the full widget rect; visibility is managed by set_current.
    const int w = size().width;
    const int h = size().height;
    for (auto * t : tabs_)
        t->set_geometry({0, 0, w, h});
}

} // namespace nx::tui
