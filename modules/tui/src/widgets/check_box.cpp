#include <nx/tui/widgets/check_box.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/input/key.hpp>
#include <nx/tui/application.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/animation/easing.hpp>

namespace nx::tui {

check_box::check_box(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
    set_fixed_height(1);
}

void check_box::set_text(std::string t)
{
    if (text_ == t) return;
    text_ = std::move(t);
    update();
}

widget::size_type check_box::size_hint() const
{
    // "[ ] " = 4 cols + label
    return { 1, 4 + static_cast<int>(text_.size()) };
}

void check_box::_init_colors() noexcept
{
    if (initialized_) return;
    initialized_ = true;
    const color bg = tui_application::instance()
                         ->get_theme().get_bg(theme_role::background);
    check_color_.set(bg);
}

void check_box::set_checked(bool c)
{
    if (checked_ == c) return;
    checked_ = c;

    _init_colors();
    const auto * app = tui_application::instance();
    const color target = checked_
        ? app->get_theme().get_color(theme_role::accent)
        : app->get_theme().get_bg(theme_role::background);
    check_color_.animate_to(target, 150, easing::ease_out);

    NX_EMIT(toggled, checked_)
    update();
}

void check_box::on_paint(painter & p)
{
    _init_colors();
    p.apply_theme_as_base(theme_role::foreground, theme_role::background);
    p.fill(" ");

    const color border_c = has_focus()
        ? p.theme_color(theme_role::border_focus)
        : p.theme_color(theme_role::border);
    const color fg_c = p.theme_color(theme_role::foreground);
    const color dim_c = p.theme_color(theme_role::foreground_dim);

    // [ ]  or  [✓]
    p.set_color(border_c);
    p.draw_char({ 0, 0 }, "[");
    p.draw_char({ 2, 0 }, "]");

    // checkmark, animated colour
    p.set_color(check_color_.value());
    p.draw_char({ 1, 0 }, "✓");

    // label
    if (!text_.empty()) {
        p.set_color(fg_c);
        p.draw_text({ 4, 0 }, text_);
    }

    (void)dim_c;
}

void check_box::on_key_press(key_event & e)
{
    if (e.code == key::enter ||
        (e.code == key::printable && e.character == U' ')) {
        toggle();
        e.accept();
    }
}

void check_box::on_mouse_press(mouse_event & e)
{
    toggle();
    e.accept();
}

void check_box::on_focus_in()  { update(); }
void check_box::on_focus_out() { update(); }

} // namespace nx::tui
