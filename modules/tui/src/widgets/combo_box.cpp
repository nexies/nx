#include <nx/tui/widgets/combo_box.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/animation/easing.hpp>
#include <nx/tui/application.hpp>

#include <algorithm>

namespace nx::tui {

// ── construction ──────────────────────────────────────────────────────────────

combo_box::combo_box(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
    set_fixed_height(1);
}

// ── Theme helpers ─────────────────────────────────────────────────────────────

color combo_box::_idle_bg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_bg(theme_role::control);
    return color::rgb(49, 50, 68);
}

color combo_box::_hover_bg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_bg(theme_role::control_hover);
    return color::rgb(69, 71, 90);
}

color combo_box::_idle_fg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_color(theme_role::foreground);
    return color::rgb(205, 214, 244);
}

color combo_box::_accent() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_color(theme_role::accent);
    return color::rgb(203, 166, 247);
}

color combo_box::_border_c() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_color(theme_role::border);
    return color::rgb(69, 71, 90);
}

color combo_box::_sel_bg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_bg(theme_role::selection);
    return color::rgb(137, 180, 250);
}

color combo_box::_sel_fg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_color(theme_role::selection_text);
    return color::rgb(30, 30, 46);
}

color combo_box::_hl_bg() const noexcept
{
    if (auto * a = tui_application::instance())
        return a->get_theme().get_bg(theme_role::control_hover);
    return color::rgb(69, 71, 90);
}

// ── Item management ───────────────────────────────────────────────────────────

void combo_box::add_item(std::string text)
{
    items_.push_back(std::move(text));
    if (current_ < 0) set_current(0);
    update();
}

void combo_box::set_items(std::vector<std::string> v)
{
    items_ = std::move(v);
    set_current(items_.empty() ? -1 : 0);
    update();
}

void combo_box::remove_item(int i)
{
    if (i < 0 || i >= (int)items_.size()) return;
    items_.erase(items_.begin() + i);
    if (current_ >= (int)items_.size()) set_current((int)items_.size() - 1);
    update();
}

void combo_box::clear()
{
    items_.clear();
    current_ = -1;
    if (expanded_) collapse(false);
    update();
}

int combo_box::count() const noexcept { return (int)items_.size(); }

const std::string & combo_box::item_text(int i) const
{
    static const std::string empty;
    if (i < 0 || i >= (int)items_.size()) return empty;
    return items_[(std::size_t)i];
}

// ── Selection ─────────────────────────────────────────────────────────────────

void combo_box::set_current(int index)
{
    if (index < -1 || index >= (int)items_.size()) return;
    if (index == current_) return;
    current_   = index;
    highlight_ = index;
    NX_EMIT(current_changed, current_)
    update();
}

// ── Expand / collapse ─────────────────────────────────────────────────────────

int combo_box::_visible_count() const noexcept
{
    return std::min((int)items_.size(), max_visible_);
}

void combo_box::expand()
{
    if (expanded_ || items_.empty()) return;
    saved_current_ = current_;
    highlight_     = current_;
    expanded_      = true;
    border_.animate_to(_accent(), 80, easing::ease_out);
    update();
}

void combo_box::collapse(bool accept)
{
    if (!expanded_) return;
    expanded_ = false;
    border_.animate_to(_border_c(), 150, easing::ease_in);
    if (accept && highlight_ >= 0) {
        const int prev = current_;
        current_ = highlight_;
        if (current_ != prev) {
            NX_EMIT(current_changed, current_)
            NX_EMIT(activated, current_)
        }
    } else {
        current_   = saved_current_;
        highlight_ = saved_current_;
    }
    update();
}

// ── Overlay API ───────────────────────────────────────────────────────────────

bool combo_box::has_overlay() const noexcept
{
    return expanded_ && !items_.empty();
}

rect<int> combo_box::overlay_rect() const noexcept
{
    if (!expanded_) return {};
    // Local coords: starts at row 1 (below header).
    // Height = top-border + n items + bottom-border.
    return { 0, 1, size().width, _visible_count() + 2 };
}

// ── Size hint ─────────────────────────────────────────────────────────────────

widget::size_type combo_box::size_hint() const
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
// on_paint draws only the header row (always height = 1).
// on_paint_overlay draws the dropdown rows when expanded.

void combo_box::on_paint(painter & p)
{
    const color bg_c  = bg_.value();
    const color fg_c  = _idle_fg();
    const color brd_c = border_.value();
    const int   w     = size().width;

    p.with(bg(bg_c) | fg(fg_c)).fill({ 0, 0, w, 1 });

    const std::string label = current_ >= 0 ? items_[(std::size_t)current_] : "";
    const int avail = std::max(0, w - 3);
    const std::string vis = label.size() > (std::size_t)avail
        ? label.substr(0, (std::size_t)avail) : label;
    p.with(fg(fg_c) | bg(bg_c)).draw_text({ 1, 0 }, vis);

    const color arrow_c = has_focus() ? brd_c : p.theme_color(theme_role::foreground_dim);
    // Show arrow and, when expanded, side borders so header connects to the dropdown frame.
    if (expanded_) {
        p.with(fg(brd_c) | bg(bg_c)).draw_char({ 0,     0 }, "│");
        p.with(fg(brd_c) | bg(bg_c)).draw_char({ w - 1, 0 }, "│");
    }
    p.with(fg(arrow_c) | bg(bg_c)).draw_char({ w - 2, 0 }, expanded_ ? "▲" : "▼");
}

void combo_box::on_paint_overlay(painter & p)
{
    // Painter local coords: (0,0) = one row below the header.
    // Layout:
    //   row 0        : ┌──────────────────────┐
    //   rows 1..n    : │ item text             │
    //   row n+1      : └──────────────────────┘
    const color fg_c   = _idle_fg();
    const color hl_bg  = _hl_bg();
    const color sel_bg = _sel_bg();
    const color sel_fg = _sel_fg();
    const color drop_bg = p.theme_bg(theme_role::control);   // slightly elevated background
    const color brd_c   = border_.value();                    // animated accent when expanded
    const int   w       = size().width;
    const int   n       = _visible_count();

    p.with(bg(drop_bg)).fill(" ");

    // Separator row connecting to the header's side borders above.
    p.with(fg(brd_c) | bg(drop_bg)).draw_char({ 0, 0 }, "├");
    for (int x = 1; x < w - 1; ++x)
        p.with(fg(brd_c) | bg(drop_bg)).draw_char({ x, 0 }, "─");
    p.with(fg(brd_c) | bg(drop_bg)).draw_char({ w - 1, 0 }, "┤");

    // Item rows
    for (int i = 0; i < n; ++i) {
        if (i >= (int)items_.size()) break;
        const int row = i + 1;

        const bool is_sel = (i == current_);
        const bool is_hl  = (i == highlight_);
        const color row_bg = is_sel ? sel_bg : (is_hl ? hl_bg : drop_bg);
        const color row_fg = is_sel ? sel_fg : fg_c;

        // Fill row, then overlay borders and text so colors are correct
        p.with(bg(row_bg) | fg(row_fg)).fill({ 0, row, w, 1 });

        p.with(fg(brd_c) | bg(drop_bg)).draw_char({ 0,     row }, "│");
        p.with(fg(brd_c) | bg(drop_bg)).draw_char({ w - 1, row }, "│");

        const std::string & txt = items_[(std::size_t)i];
        const int avail = std::max(0, w - 3);
        const std::string vis = txt.size() > (std::size_t)avail
            ? txt.substr(0, (std::size_t)avail) : txt;
        p.with(fg(row_fg) | bg(row_bg)).draw_text({ 1, row }, vis);
    }

    // Bottom border
    const int bot = n + 1;
    p.with(fg(brd_c) | bg(drop_bg)).draw_char({ 0, bot }, "└");
    for (int x = 1; x < w - 1; ++x)
        p.with(fg(brd_c) | bg(drop_bg)).draw_char({ x, bot }, "─");
    p.with(fg(brd_c) | bg(drop_bg)).draw_char({ w - 1, bot }, "┘");
}

// ── Keyboard ──────────────────────────────────────────────────────────────────

void combo_box::on_key_press(key_event & e)
{
    if (!expanded_) {
        switch (e.code) {
        case key::enter:
        case key::arrow_down:
            expand();
            break;
        case key::arrow_up:
            if (current_ > 0) { set_current(current_ - 1); NX_EMIT(activated, current_) }
            break;
        default:
            return;
        }
    } else {
        switch (e.code) {
        case key::arrow_up:
            if (highlight_ > 0) { --highlight_; update(); }
            break;
        case key::arrow_down:
            if (highlight_ < (int)items_.size() - 1) { ++highlight_; update(); }
            break;
        case key::home:
            highlight_ = 0; update();
            break;
        case key::end:
            highlight_ = std::max(0, (int)items_.size() - 1); update();
            break;
        case key::enter:
            collapse(true);
            break;
        case key::escape:
            collapse(false);
            break;
        default:
            return;
        }
    }
    e.accept();
}

// ── Mouse ─────────────────────────────────────────────────────────────────────
//
// When expanded, clicks on the dropdown arrive via _widget_at_overlay in screen.
// e.position is in LOCAL widget coords (y=0 = header, y=1..n = overlay rows).

void combo_box::on_mouse_press(mouse_event & e)
{
    if (e.button != mouse_button::left) return;
    if (e.position.y == 0) {
        expanded_ ? collapse(true) : expand();
    } else if (expanded_) {
        // Overlay layout: row 1 = top border, rows 2..n+1 = items, row n+2 = bottom border.
        // e.position.y is local (0 = header), so item index = y - 2.
        const int idx = e.position.y - 2;
        if (idx >= 0 && idx < (int)items_.size()) {
            highlight_ = idx;
            collapse(true);
        }
    }
    e.accept();
}

void combo_box::on_mouse_move(mouse_event & e)
{
    if (!expanded_) return;
    const int idx = e.position.y - 2;
    if (idx >= 0 && idx < (int)items_.size() && idx != highlight_) {
        highlight_ = idx;
        update();
    }
}

void combo_box::on_mouse_enter(mouse_event & e)
{
    widget::on_mouse_enter(e);
    bg_.animate_to(_hover_bg(), 100, easing::ease_out);
}

void combo_box::on_mouse_leave(mouse_event & e)
{
    widget::on_mouse_leave(e);
    bg_.animate_to(_idle_bg(), 150, easing::ease_in);
}

void combo_box::on_focus_in()
{
    widget::on_focus_in();
    update();
}

void combo_box::on_focus_out()
{
    widget::on_focus_out();
    if (expanded_) collapse(true);
}

} // namespace nx::tui
