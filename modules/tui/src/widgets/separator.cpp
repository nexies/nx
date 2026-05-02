#include <nx/tui/widgets/separator.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/types/theme_role.hpp>

namespace nx::tui {

separator::separator(nx::core::object * parent, orientation o)
    : widget(parent), orient_(o)
{
    set_focus_policy(focus_policy::no_focus);
    set_transparent(true);
    if (o == orientation::horizontal) set_fixed_height(1);
    else                              set_fixed_width(1);
}

void separator::set_orientation(orientation o)
{
    if (orient_ == o) return;
    orient_ = o;
    if (o == orientation::horizontal) set_fixed_height(1);
    else                              set_fixed_width(1);
    update();
}

widget::size_type separator::size_hint() const
{
    const auto eh = explicit_hint();
    if (orient_ == orientation::horizontal)
        return { 1, eh.width > 0 ? eh.width : size().width };
    else
        return { eh.height > 0 ? eh.height : size().height, 1 };
}

void separator::on_paint(painter & p)
{
    p.set_color(p.theme_color(theme_role::border));
    const int w = size().width;
    const int h = size().height;
    if (orient_ == orientation::horizontal) {
        for (int x = 0; x < w; ++x)
            p.draw_char({ x, 0 }, "─");
    } else {
        for (int y = 0; y < h; ++y)
            p.draw_char({ 0, y }, "│");
    }
}

} // namespace nx::tui
