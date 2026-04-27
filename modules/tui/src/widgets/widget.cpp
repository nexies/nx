#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/graphics/painter.hpp>

#include <algorithm>

namespace nx::tui {

widget::widget(object * parent)
    : object(parent)
{}

void widget::set_geometry(const rect<int> & r)
{
    const auto old_size = geometry_.size();
    geometry_ = r;
    dirty_ = true;
    const auto new_size = r.size();
    if (old_size.height != new_size.height || old_size.width != new_size.width) {
        on_resize(old_size, new_size);
    }
    NX_EMIT(geometry_changed)
}

std::vector<widget *> widget::child_widgets() const
{
    std::vector<widget *> result;
    for (auto * child : children()) {
        if (auto * w = dynamic_cast<widget *>(child)) {
            result.push_back(w);
        }
    }
    return result;
}

widget::size_type widget::size_hint() const
{
    const auto geo = geometry_.size();
    return {
        hint_.height > 0 ? hint_.height : geo.height,
        hint_.width  > 0 ? hint_.width  : geo.width
    };
}

widget::size_type widget::minimum_size() const
{
    return {
        v_policy_ == size_policy::fixed ? size_hint().height : 0,
        h_policy_ == size_policy::fixed ? size_hint().width  : 0
    };
}

void widget::_apply_layout()               {}
void widget::on_paint(painter &)           {}
void widget::on_key_press(key_event &)     {}
void widget::on_key_release(key_event &)   {}
void widget::on_mouse_press(mouse_event &) {}
void widget::on_mouse_release(mouse_event &) {}
void widget::on_mouse_move(mouse_event &)  {}
void widget::on_wheel(mouse_event &)       {}
void widget::on_resize(size_type, size_type) {}
void widget::on_focus_in()  { focused_ = true;  update(); }
void widget::on_focus_out() { focused_ = false; update(); }

// ── event filters ─────────────────────────────────────────────────────────────

void widget::install_event_filter(event_filter * f)
{
    if (f && std::find(filters_.begin(), filters_.end(), f) == filters_.end())
        filters_.push_back(f);
}

void widget::remove_event_filter(event_filter * f)
{
    filters_.erase(std::remove(filters_.begin(), filters_.end(), f),
                   filters_.end());
}

bool widget::_run_filters_key(key_event & e)
{
    for (auto * f : filters_)
        if (f->filter_key(e)) return true;
    return false;
}

bool widget::_run_filters_mouse(mouse_event & e)
{
    for (auto * f : filters_)
        if (f->filter_mouse(e)) return true;
    return false;
}

} // namespace nx::tui
