#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/graphics/painter.hpp>

#include <algorithm>

namespace nx::tui {

widget::widget(object * parent)
    : object(parent)
{}

void widget::set_geometry(const rect<int> & r)
{
    if (geometry_ == r) return;

    const auto old_size = geometry_.size();
    geometry_ = r;
    update();   // marks dirty_ + propagates subtree_dirty_ up
    const auto new_size = r.size();
    if (old_size.height != new_size.height || old_size.width != new_size.width) {
        on_resize(old_size, new_size);
    }
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

void widget::update() noexcept
{
    dirty_ = true;
    // Walk up the parent chain marking subtree_dirty_ = true.
    // Stop as soon as we hit an ancestor that's already marked — it means
    // all further ancestors were already marked by a prior update() call.
    object * p = this;
    while (p) {
        widget * w = dynamic_cast<widget *>(p);
        if (!w) break;
        if (w->subtree_dirty_) break;
        w->subtree_dirty_ = true;
        p = p->parent();
    }
}

void widget::_apply_layout()               {}
void widget::on_paint(painter &)           {}
void widget::on_key_press(key_event &)     {}
void widget::on_key_release(key_event &)   {}
void widget::on_mouse_press(mouse_event &)  {}
void widget::on_mouse_release(mouse_event &) {}
void widget::on_mouse_move(mouse_event &)   {}
void widget::on_wheel(mouse_event &)        {}
void widget::on_mouse_enter(mouse_event &)  { hovered_ = true;  update(); }
void widget::on_mouse_leave(mouse_event &)  { hovered_ = false; update(); }
void widget::on_resize(size_type, size_type) {}
void widget::on_focus_in()  { focused_ = true;  update(); }
void widget::on_focus_out() { focused_ = false; update(); }

bool widget::has_focused_descendant() const noexcept
{
    if (focused_) return true;
    for (auto * child : child_widgets())
        if (child->has_focused_descendant()) return true;
    return false;
}

bool widget::on_event(nx::core::event & e)
{
    switch (e.type()) {
    case key_event::type_press:
        on_key_press(static_cast<key_event &>(e));
        return e.is_accepted();
    case key_event::type_release:
        on_key_release(static_cast<key_event &>(e));
        return e.is_accepted();
    case mouse_event::TYPE: {
        auto & me = static_cast<mouse_event &>(e);
        switch (me.action) {
        case mouse_action::press:   on_mouse_press(me);   break;
        case mouse_action::release: on_mouse_release(me); break;
        case mouse_action::move:    on_mouse_move(me);    break;
        case mouse_action::wheel:   on_wheel(me);         break;
        case mouse_action::enter:   on_mouse_enter(me);   break;
        case mouse_action::leave:   on_mouse_leave(me);   break;
        }
        return e.is_accepted();
    }
    default:
        return false;
    }
}

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
