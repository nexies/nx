#include <nx/tui/widgets/widget.hpp>
#include <nx/tui/layouts/layout.hpp>
#include <nx/tui/graphics/painter.hpp>

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

} // namespace nx::tui
