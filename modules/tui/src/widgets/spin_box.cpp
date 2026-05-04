#include <nx/tui/widgets/spin_box.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/animation/easing.hpp>
#include <nx/tui/application.hpp>

#include <algorithm>
#include <fmt/format.h>

namespace nx::tui {

// ── construction ──────────────────────────────────────────────────────────────

spin_box::spin_box(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
    set_fixed_height(1);
}

// ── Theme color helpers ───────────────────────────────────────────────────────

color spin_box::_idle_bg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_bg(theme_role::control);
    return color::rgb(49, 50, 68);
}

color spin_box::_hover_bg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_bg(theme_role::control_hover);
    return color::rgb(69, 71, 90);
}

color spin_box::_idle_fg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_color(theme_role::foreground);
    return color::rgb(205, 214, 244);
}

color spin_box::_accent() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_color(theme_role::accent);
    return color::rgb(203, 166, 247);
}

// ── Public API ────────────────────────────────────────────────────────────────

void spin_box::set_range(int lo, int hi)
{
    min_ = lo;
    max_ = std::max(lo, hi);
    set_value(std::clamp(value_, min_, max_));
    update();
}

void spin_box::set_value(int v)
{
    _set_value_clamped(v);
}

widget::size_type spin_box::size_hint() const
{
    const int w = explicit_hint().width > 0 ? explicit_hint().width : size().width;
    return { 1, w };
}

// ── Helpers ───────────────────────────────────────────────────────────────────

void spin_box::_set_value_clamped(int v)
{
    const int clamped = std::clamp(v, min_, max_);
    if (clamped == value_) return;
    value_ = clamped;
    NX_EMIT(value_changed, value_)
    update();
}

void spin_box::_commit_edit()
{
    if (editing_ && !edit_buf_.empty()) {
        try { _set_value_clamped(std::stoi(edit_buf_)); }
        catch (...) {}
    }
    editing_  = false;
    edit_buf_.clear();
    update();
}

void spin_box::_cancel_edit()
{
    editing_  = false;
    edit_buf_.clear();
    update();
}

std::string spin_box::_display_text() const
{
    if (editing_) return prefix_ + edit_buf_ + suffix_;
    return prefix_ + fmt::format("{}", value_) + suffix_;
}

// ── Painting ──────────────────────────────────────────────────────────────────
//
// Layout:  │ [prefix value suffix] ▲▼ │
//   ▲▼ visible only when focused, occupy last 2 columns.

void spin_box::on_paint(painter & p)
{
    const color bg_c     = bg_.value();
    const color fg_c     = _idle_fg();
    const color accent_c = _accent();

    p.apply_style(bg(bg_c) | fg(fg_c));
    p.fill();

    const int w = size().width;
    if (w <= 0) return;

    const std::string display = _display_text();

    if (has_focus()) {
        // Reserve last 2 columns for ▲▼
        const int avail = std::max(0, w - 2);
        const std::string vis = display.size() > (std::size_t)avail
            ? display.substr(0, (std::size_t)avail) : display;
        p.draw_text({ 1, 0 }, vis);

        p.with(fg(accent_c) | bg(bg_c))
         .draw_char({ w - 2, 0 }, "▲");
        p.with(fg(accent_c) | bg(bg_c))
         .draw_char({ w - 1, 0 }, "▼");
    } else {
        const std::string vis = display.size() > (std::size_t)(w - 2)
            ? display.substr(0, (std::size_t)(w - 2)) : display;
        p.draw_text({ 1, 0 }, vis);
    }
}

// ── Keyboard ──────────────────────────────────────────────────────────────────

void spin_box::on_key_press(key_event & e)
{
    switch (e.code) {
    case key::arrow_up:
        _cancel_edit();
        _set_value_clamped(value_ + step_);
        break;
    case key::arrow_down:
        _cancel_edit();
        _set_value_clamped(value_ - step_);
        break;
    case key::page_up:
        _cancel_edit();
        _set_value_clamped(value_ + step_ * 10);
        break;
    case key::page_down:
        _cancel_edit();
        _set_value_clamped(value_ - step_ * 10);
        break;
    case key::home:
        _cancel_edit();
        _set_value_clamped(min_);
        break;
    case key::end:
        _cancel_edit();
        _set_value_clamped(max_);
        break;
    case key::enter:
        _commit_edit();
        break;
    case key::escape:
        _cancel_edit();
        break;
    case key::backspace:
        if (editing_ && !edit_buf_.empty()) {
            edit_buf_.pop_back();
            update();
        }
        break;
    case key::printable: {
        const char32_t cp = e.character;
        const bool neg = (cp == U'-' && edit_buf_.empty() && min_ < 0);
        if ((cp >= U'0' && cp <= U'9') || neg) {
            if (!editing_) { editing_ = true; edit_buf_.clear(); }
            edit_buf_ += static_cast<char>(cp);
            update();
        } else {
            return;
        }
        break;
    }
    default:
        return;
    }
    e.accept();
}

// ── Mouse ─────────────────────────────────────────────────────────────────────

void spin_box::on_mouse_press(mouse_event & e)
{
    if (e.button != mouse_button::left) return;
    const int w = size().width;
    if (e.position.x == w - 2) { _cancel_edit(); _set_value_clamped(value_ + step_); }
    else if (e.position.x == w - 1) { _cancel_edit(); _set_value_clamped(value_ - step_); }
    e.accept();
}

void spin_box::on_mouse_enter(mouse_event & e)
{
    widget::on_mouse_enter(e);
    bg_.animate_to(_hover_bg(), 100, easing::ease_out);
}

void spin_box::on_mouse_leave(mouse_event & e)
{
    widget::on_mouse_leave(e);
    bg_.animate_to(_idle_bg(), 150, easing::ease_in);
}

void spin_box::on_focus_in()
{
    widget::on_focus_in();
    update();
}

void spin_box::on_focus_out()
{
    widget::on_focus_out();
    _commit_edit();
}

} // namespace nx::tui
