#include <nx/tui/widgets/button.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/animation/easing.hpp>

#include <algorithm>

namespace nx::tui {

// ── border character sets (same as frame.cpp) ─────────────────────────────────

namespace {

struct border_chars {
    const char * h, * v, * tl, * tr, * bl, * br;
};

border_chars chars_for(border_style s) noexcept
{
    switch (s) {
    case border_style::single:  return { "─","│","┌","┐","└","┘" };
    case border_style::double_: return { "═","║","╔","╗","╚","╝" };
    case border_style::rounded: return { "─","│","╭","╮","╰","╯" };
    case border_style::thick:   return { "━","┃","┏","┓","┗","┛" };
    case border_style::dashed:  return { "╌","╎","┌","┐","└","┘" };
    default:                    return { " "," "," "," "," "," " };
    }
}

} // namespace

// ── button ────────────────────────────────────────────────────────────────────

button::button(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
    set_fixed_height(3);
}

// ── Public API ────────────────────────────────────────────────────────────────

void button::click() { NX_EMIT(clicked) }

void button::set_text(std::string t)
{
    if (text_ == t) return;
    text_ = std::move(t);
    update();
}

void button::set_base_color(color bg, color fg)
{
    base_bg_ = bg;
    base_fg_ = fg;

    const color white = color::rgb(255, 255, 255);
    hover_bg_ = color::interpolate(0.15f, bg, white);
    press_bg_ = color::interpolate(0.30f, bg, white);

    bg_.set(base_bg_);
    border_.set(base_fg_);
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

    // Fill background.
    p.apply_style(bg(bg_.value()));
    p.fill(" ");

    if (w < 2 || h < 2) return;

    const auto bc = chars_for(border_style_);

    // Border in animated border color.
    p.apply_style(fg(border_.value()) | bg(bg_.value()));

    p.draw_char({0,     0    }, bc.tl);
    p.draw_char({w - 1, 0    }, bc.tr);
    p.draw_char({0,     h - 1}, bc.bl);
    p.draw_char({w - 1, h - 1}, bc.br);

    for (int col = 1; col < w - 1; ++col) {
        p.draw_char({col, 0    }, bc.h);
        p.draw_char({col, h - 1}, bc.h);
    }
    for (int row = 1; row < h - 1; ++row) {
        p.draw_char({0,     row}, bc.v);
        p.draw_char({w - 1, row}, bc.v);
    }

    // Label centered in the inner area.
    if (text_.empty() || h < 3) return;

    const int row   = h / 2;
    const int avail = w - 2;
    if (avail <= 0) return;

    const std::string vis = text_.size() > static_cast<std::size_t>(avail)
        ? text_.substr(0, static_cast<std::size_t>(avail))
        : text_;
    const int x = 1 + (avail - static_cast<int>(vis.size())) / 2;

    p.apply_style(fg(base_fg_) | bg(bg_.value()));
    if (has_focus()) p.enable_style(pixel_style_flag::bold);
    p.draw_text({x, row}, vis);
    if (has_focus()) p.disable_style(pixel_style_flag::bold);
}

// ── Focus ─────────────────────────────────────────────────────────────────────

void button::on_focus_in()
{
    widget::on_focus_in();
    update(); // repaint so text becomes bold
}

void button::on_focus_out()
{
    widget::on_focus_out();
    update(); // repaint so text loses bold
}

// ── Mouse ─────────────────────────────────────────────────────────────────────

void button::on_mouse_enter(mouse_event & e)
{
    widget::on_mouse_enter(e);
    bg_.animate_to(hover_bg_, 150, easing::ease_out);
}

void button::on_mouse_leave(mouse_event & e)
{
    widget::on_mouse_leave(e);
    bg_.animate_to(base_bg_, 200, easing::ease_in);
}

void button::on_mouse_press(mouse_event & e)
{
    if (e.button != mouse_button::left) return;
    bg_.animate_to(press_bg_, 60, easing::ease_out);
    border_.animate_to(accent_, 60, easing::ease_out);
    NX_EMIT(clicked)
    e.accept();
}

void button::on_mouse_release(mouse_event & e)
{
    if (e.button == mouse_button::left) {
        bg_.animate_to(_target_bg(), 200, easing::ease_in);
        border_.animate_to(base_fg_,  300, easing::ease_in);
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

color button::_target_bg() const noexcept
{
    return is_hovered() ? hover_bg_ : base_bg_;
}

void button::_do_click()
{
    // Flash to press state, then animate back once the flash finishes.
    bg_.animate_to(press_bg_, 60, easing::ease_out);
    border_.animate_to(accent_, 60, easing::ease_out);

    nx::core::connect(&bg_.raw(), &animator::finished, this,
        [this]() { bg_.animate_to(_target_bg(), 200, easing::ease_out); },
        nx::core::connection_type::auto_t,
        nx::core::connection_flag::single_shot);

    nx::core::connect(&border_.raw(), &animator::finished, this,
        [this]() { border_.animate_to(base_fg_, 300, easing::ease_in); },
        nx::core::connection_type::auto_t,
        nx::core::connection_flag::single_shot);

    NX_EMIT(clicked)
}

} // namespace nx::tui
