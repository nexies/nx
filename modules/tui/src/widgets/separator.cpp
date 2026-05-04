#include <nx/tui/widgets/separator.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/types/style_option.hpp>

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
    auto styled = p.with(fg(p.theme_color(theme_role::border)));
    if (orient_ == orientation::horizontal)
        styled.draw_hline(0);
    else
        styled.draw_vline(0);
}

} // namespace nx::tui
