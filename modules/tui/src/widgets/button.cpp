#include <nx/tui/widgets/button.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/application.hpp>
#include <nx/tui/animation/easing.hpp>

namespace nx::tui {

// ── button ────────────────────────────────────────────────────────────────────

button::button(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
    set_fixed_height(3);
}

// ── Color helpers ─────────────────────────────────────────────────────────────

color button::_idle_bg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_bg(theme_role::control);
    return color::rgb(49, 50, 68);
}

color button::_hover_bg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_bg(theme_role::control_hover);
    return color::rgb(69, 71, 90);
}

color button::_press_bg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_bg(theme_role::control_active);
    return color::rgb(88, 91, 112);
}

color button::_idle_fg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_color(theme_role::control);
    return color::rgb(205, 214, 244);
}

color button::_accent() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_color(theme_role::accent);
    return color::rgb(203, 166, 247);
}

color button::_target_bg() const noexcept
{
    return is_hovered() ? _hover_bg() : _idle_bg();
}

// ── Public API ────────────────────────────────────────────────────────────────

void button::click() { NX_EMIT(clicked) }

void button::set_text(std::string t)
{
    if (text_ == t) return;
    text_ = std::move(t);
    update();
}

widget::size_type button::size_hint() const
{
    const auto h = explicit_hint();
    const auto s = size();
    return {
        (h.height > 0 ? h.height : s.height),
        (h.width  > 0 ? h.width  : s.width)
    };
}

// ── Painting ──────────────────────────────────────────────────────────────────

void button::on_paint(painter & p)
{
    const int w = size().width;
    const int h = size().height;

    const color bg_c     = bg_.value();
    const color border_c = border_.value();
    const color fg_c     = _idle_fg();

    p.with(bg(bg_c)).fill();

    if (w < 2 || h < 2) return;

    p.with(fg(border_c) | bg(bg_c)).draw_border(border_style_);

    if (text_.empty() || h < 3) return;

    const int avail = w - 2;
    if (avail <= 0) return;

    const std::string vis = text_.size() > static_cast<std::size_t>(avail)
        ? text_.substr(0, static_cast<std::size_t>(avail))
        : text_;
    const int x = 1 + (avail - static_cast<int>(vis.size())) / 2;

    style_option label_s = fg(fg_c) | bg(bg_c);
    if (has_focus()) label_s |= bold() | underline();
    p.with(label_s).draw_text({ x, h / 2 }, vis);
}

// ── Focus ─────────────────────────────────────────────────────────────────────

void button::on_focus_in()
{
    widget::on_focus_in();
    update();
}

void button::on_focus_out()
{
    widget::on_focus_out();
    update();
}

// ── Mouse ─────────────────────────────────────────────────────────────────────

void button::on_mouse_enter(mouse_event & e)
{
    widget::on_mouse_enter(e);
    bg_.animate_to(_hover_bg(), 150, easing::ease_out);
}

void button::on_mouse_leave(mouse_event & e)
{
    widget::on_mouse_leave(e);
    bg_.animate_to(_idle_bg(), 200, easing::ease_in);
}

void button::on_mouse_press(mouse_event & e)
{
    if (e.button != mouse_button::left) return;
    bg_.animate_to(_press_bg(), 60, easing::ease_out);
    border_.animate_to(_accent(), 60, easing::ease_out);
    NX_EMIT(clicked)
    e.accept();
}

void button::on_mouse_release(mouse_event & e)
{
    if (e.button == mouse_button::left) {
        bg_.animate_to(_target_bg(), 200, easing::ease_in);
        border_.animate_to(_idle_fg(), 300, easing::ease_in);
        e.accept();
    }
    widget::on_mouse_release(e);
}

// ── Keyboard ──────────────────────────────────────────────────────────────────

void button::on_key_press(key_event & e)
{
    if (e.code == key::enter ||
        (e.code == key::printable && e.character == U' '))
    {
        _do_click();
        e.accept();
    }
}

// ── Helpers ───────────────────────────────────────────────────────────────────

void button::_do_click()
{
    bg_.animate_to(_press_bg(), 60, easing::ease_out);
    border_.animate_to(_accent(), 60, easing::ease_out);

    nx::core::connect(&bg_.raw(), &animator::finished, this,
        [this]() { bg_.animate_to(_target_bg(), 200, easing::ease_out); },
        nx::core::connection_type::auto_t,
        nx::core::connection_flag::single_shot);

    nx::core::connect(&border_.raw(), &animator::finished, this,
        [this]() { border_.animate_to(_idle_fg(), 300, easing::ease_in); },
        nx::core::connection_type::auto_t,
        nx::core::connection_flag::single_shot);

    NX_EMIT(clicked)
}

} // namespace nx::tui
