#include <nx/tui/widgets/slider.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/theme_role.hpp>

#include <algorithm>

namespace nx::tui {

// ── construction ──────────────────────────────────────────────────────────────

slider::slider(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
    set_fixed_height(1);
    set_horizontal_policy(size_policy::expanding);
}

// ── Public API ────────────────────────────────────────────────────────────────

void slider::set_range(int lo, int hi)
{
    min_ = lo;
    max_ = std::max(lo, hi);
    set_value(std::clamp(value_, min_, max_));
    NX_EMIT(range_changed, min_, max_)
    update();
}

void slider::set_value(int v)
{
    _set_value_clamped(v);
}

void slider::set_orientation(orientation o)
{
    orient_ = o;
    if (o == orientation::horizontal) {
        set_fixed_height(1);
        set_horizontal_policy(size_policy::expanding);
        set_vertical_policy(size_policy::fixed);
    } else {
        set_fixed_width(1);
        set_vertical_policy(size_policy::expanding);
        set_horizontal_policy(size_policy::fixed);
    }
    update();
}

// ── Helpers ───────────────────────────────────────────────────────────────────

void slider::_set_value_clamped(int v)
{
    const int clamped = std::clamp(v, min_, max_);
    if (clamped == value_) return;
    value_ = clamped;
    NX_EMIT(value_changed, value_)
    update();
}

int slider::_track_len() const noexcept
{
    return orient_ == orientation::horizontal
        ? std::max(1, size().width)
        : std::max(1, size().height);
}

int slider::_value_to_pos(int v) const noexcept
{
    const int range = max_ - min_;
    if (range <= 0) return 0;
    const int len = _track_len() - 1;
    return (v - min_) * len / range;
}

int slider::_pos_to_value(int pos) const noexcept
{
    const int len = _track_len() - 1;
    if (len <= 0) return min_;
    return min_ + pos * (max_ - min_) / len;
}

// ── Size hint ─────────────────────────────────────────────────────────────────

widget::size_type slider::size_hint() const
{
    const auto h = explicit_hint();
    const auto s = size();
    return {
        h.height > 0 ? h.height : s.height,
        h.width  > 0 ? h.width  : s.width
    };
}

// ── Painting ──────────────────────────────────────────────────────────────────
//
// Horizontal: ━━━━━━━━━━●──────────────────
//   filled = accent, thumb = accent, empty = border
// Vertical:  │ above, ● thumb, ┃ below (filled)

void slider::on_paint(painter & p)
{
    p.apply_theme_as_base(theme_role::foreground, theme_role::background);
    p.fill();

    const color accent_c = p.theme_color(theme_role::accent);
    const color border_c = p.theme_color(has_focus() ? theme_role::border_focus
                                                     : theme_role::border);
    const color thumb_c  = has_focus() ? accent_c
                                       : p.theme_color(theme_role::foreground_dim);

    const int thumb_pos = _value_to_pos(value_);
    const int track     = _track_len();

    if (orient_ == orientation::horizontal) {
        for (int i = 0; i < track; ++i) {
            if (i == thumb_pos)
                p.with(fg(thumb_c)).draw_char({ i, 0 }, "●");
            else if (i < thumb_pos)
                p.with(fg(accent_c)).draw_char({ i, 0 }, "━");
            else
                p.with(fg(border_c)).draw_char({ i, 0 }, "─");
        }
    } else {
        for (int i = 0; i < track; ++i) {
            if (i == thumb_pos)
                p.with(fg(thumb_c)).draw_char({ 0, i }, "●");
            else if (i > thumb_pos)
                p.with(fg(accent_c)).draw_char({ 0, i }, "┃");
            else
                p.with(fg(border_c)).draw_char({ 0, i }, "│");
        }
    }
}

// ── Keyboard ──────────────────────────────────────────────────────────────────

void slider::on_key_press(key_event & e)
{
    const bool horiz = orient_ == orientation::horizontal;

    switch (e.code) {
    case key::arrow_left:  if ( horiz) _set_value_clamped(value_ - step_);      break;
    case key::arrow_right: if ( horiz) _set_value_clamped(value_ + step_);      break;
    case key::arrow_up:    if (!horiz) _set_value_clamped(value_ - step_);      break;
    case key::arrow_down:  if (!horiz) _set_value_clamped(value_ + step_);      break;
    case key::page_up:                 _set_value_clamped(value_ - page_step_); break;
    case key::page_down:               _set_value_clamped(value_ + page_step_); break;
    case key::home:                    _set_value_clamped(min_);                 break;
    case key::end:                     _set_value_clamped(max_);                 break;
    default: return;
    }
    e.accept();
}

// ── Mouse ─────────────────────────────────────────────────────────────────────

void slider::on_mouse_press(mouse_event & e)
{
    if (e.button != mouse_button::left) return;
    const int pos = orient_ == orientation::horizontal
        ? e.position.x : e.position.y;
    _set_value_clamped(_pos_to_value(pos));
    e.accept();
}

void slider::on_wheel(mouse_event & e)
{
    auto delta = e.button == mouse_button::wheel_down ? 3 : -3;
    _set_value_clamped(value_ - delta * step_);
    e.accept();
}

void slider::on_focus_in()  { widget::on_focus_in();  update(); }
void slider::on_focus_out() { widget::on_focus_out(); update(); }

} // namespace nx::tui
