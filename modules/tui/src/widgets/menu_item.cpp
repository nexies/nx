#include <nx/tui/widgets/menu_item.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/animation/easing.hpp>
#include <nx/tui/application.hpp>

namespace nx::tui {

// ── construction ──────────────────────────────────────────────────────────────

menu_item::menu_item(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::no_focus);
    set_fixed_height(1);
}

// ── Theme color helpers ───────────────────────────────────────────────────────

color menu_item::_normal_bg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_bg(theme_role::background);
    return color::rgb(30, 30, 46);
}

color menu_item::_hover_bg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_bg(theme_role::control);
    return color::rgb(49, 50, 68);
}

color menu_item::_selected_bg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_bg(theme_role::selection);
    return color::rgb(137, 180, 250);
}

color menu_item::_selected_fg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_color(theme_role::selection_text);
    return color::rgb(30, 30, 46);
}

color menu_item::_normal_fg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_color(theme_role::foreground);
    return color::rgb(205, 214, 244);
}

color menu_item::_target_bg() const noexcept
{
    if (selected_)    return _selected_bg();
    if (is_hovered()) return _hover_bg();
    return _normal_bg();
}

// ── Public API ────────────────────────────────────────────────────────────────

void menu_item::set_text(std::string t)
{
    if (text_ == t) return;
    text_ = std::move(t);
    update();
}

void menu_item::set_selected(bool s)
{
    if (selected_ == s) return;
    selected_ = s;
    _update_bg();
}

widget::size_type menu_item::size_hint() const
{
    const int w = explicit_hint().width > 0 ? explicit_hint().width : size().width;
    return { 1, w };
}

// ── Private helpers ───────────────────────────────────────────────────────────

void menu_item::_update_bg()
{
    bg_.animate_to(_target_bg(), 120, easing::ease_out);
    update();
}

// ── Painting ──────────────────────────────────────────────────────────────────

void menu_item::on_paint(painter & p)
{
    const color bg_c = bg_.value();
    const color fg_c = selected_ ? _selected_fg() : _normal_fg();

    p.apply_style(bg(bg_c) | fg(fg_c));
    p.fill();

    if (!text_.empty())
        p.draw_text({ 1, 0 }, text_);
}

// ── Mouse ─────────────────────────────────────────────────────────────────────

void menu_item::on_mouse_press(mouse_event & e)
{
    if (e.button == mouse_button::left) {
        NX_EMIT(clicked)
        e.accept();
    }
}

void menu_item::on_mouse_enter(mouse_event & e)
{
    widget::on_mouse_enter(e);
    _update_bg();
}

void menu_item::on_mouse_leave(mouse_event & e)
{
    widget::on_mouse_leave(e);
    _update_bg();
}

} // namespace nx::tui
