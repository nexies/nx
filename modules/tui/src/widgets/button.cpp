#include <nx/tui/widgets/button.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/types/style_option.hpp>

namespace nx::tui {

button::button(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
    set_fixed_height(1);
}

void button::click() {
    NX_EMIT(clicked)
}

widget::size_type button::size_hint() const
{
    const int w = explicit_hint().width > 0 ? explicit_hint().width : size().width;
    return { 1, w };
}

void button::set_text(std::string t)
{
    if (text_ == t) return;
    text_ = std::move(t);
    update();
}

void button::on_paint(painter & p)
{
    if (has_focus()) {
        p.apply_style(inverted());
    }

    p.fill(" ");

    const int w = size().width;
    if (w <= 0 || text_.empty()) return;

    std::string visible = text_.size() > static_cast<std::size_t>(w)
        ? text_.substr(0, static_cast<std::size_t>(w))
        : text_;

    const int x = (w - static_cast<int>(visible.size())) / 2;
    p.draw_text({ x < 0 ? 0 : x, 0 }, visible);
}

void button::on_key_press(key_event & e)
{
    if (e.code == key::enter || e.code == key::printable && e.character == U' ') {
        NX_EMIT(clicked)
    }
}

void button::on_mouse_press(mouse_event & e)
{
    if (e.button == mouse_button::left) {
        NX_EMIT(clicked)
    }
}

} // namespace nx::tui
