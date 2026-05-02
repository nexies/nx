#include <nx/tui/widgets/radio_button.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/input/key.hpp>
#include <nx/tui/application.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/animation/easing.hpp>

#include <algorithm>

namespace nx::tui {

// ── radio_group ───────────────────────────────────────────────────────────────

radio_group::radio_group(nx::core::object * parent)
    : nx::core::object(parent)
{}

void radio_group::add(radio_button * btn)
{
    if (!btn) return;
    if (std::find(buttons_.begin(), buttons_.end(), btn) != buttons_.end()) return;
    btn->group_ = this;
    buttons_.push_back(btn);
    // If this is the first button and nothing is selected, select it.
    if (current_ < 0) {
        current_ = 0;
        btn->set_checked(true);
    }
}

void radio_group::set_current(int idx)
{
    if (buttons_.empty()) return;
    idx = std::clamp(idx, 0, static_cast<int>(buttons_.size()) - 1);
    if (idx == current_) return;
    buttons_[idx]->set_checked(true); // will call _button_selected
}

void radio_group::_button_selected(radio_button * src)
{
    const auto it = std::find(buttons_.begin(), buttons_.end(), src);
    if (it == buttons_.end()) return;
    const int new_idx = static_cast<int>(it - buttons_.begin());

    for (int i = 0; i < static_cast<int>(buttons_.size()); ++i) {
        if (i != new_idx)
            buttons_[i]->_deselect();
    }
    current_ = new_idx;
    NX_EMIT(selection_changed, current_)
}

// ── radio_button ──────────────────────────────────────────────────────────────

radio_button::radio_button(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
    set_fixed_height(1);
}

void radio_button::set_text(std::string t)
{
    if (text_ == t) return;
    text_ = std::move(t);
    update();
}

widget::size_type radio_button::size_hint() const
{
    // "◯ " = 3 cols (glyph is 3 bytes but 1 cell wide... actually ◯ is 1 wide) + space + label
    return { 1, 3 + static_cast<int>(text_.size()) };
}

void radio_button::_init_colors() noexcept
{
    if (initialized_) return;
    initialized_ = true;
    const color bg = tui_application::instance()
                         ->get_theme().get_bg(theme_role::background);
    dot_color_.set(bg);
}

void radio_button::set_checked(bool c)
{
    if (checked_ == c) return;
    checked_ = c;

    _init_colors();
    const auto * app = tui_application::instance();
    const color target = checked_
        ? app->get_theme().get_color(theme_role::highlight)
        : app->get_theme().get_bg(theme_role::background);
    dot_color_.animate_to(target, 150, easing::ease_out);

    NX_EMIT(toggled, checked_)

    if (checked_ && group_)
        group_->_button_selected(this);

    update();
}

void radio_button::_deselect()
{
    if (!checked_) return;
    checked_ = false;
    _init_colors();
    const color bg = tui_application::instance()
                         ->get_theme().get_bg(theme_role::background);
    dot_color_.animate_to(bg, 150, easing::ease_out);
    NX_EMIT(toggled, false)
    update();
}

void radio_button::on_paint(painter & p)
{
    _init_colors();
    p.apply_theme_as_base(theme_role::foreground, theme_role::background);
    p.fill(" ");

    const color border_c = has_focus()
        ? p.theme_color(theme_role::border_focus)
        : p.theme_color(theme_role::border);
    const color fg_c = p.theme_color(theme_role::foreground);

    // Outer ring
    p.set_color(border_c);
    p.draw_char({ 0, 0 }, checked_ ? "◉" : "◯");

    // Label
    if (!text_.empty()) {
        p.set_color(fg_c);
        p.draw_text({ 2, 0 }, text_);
    }
}

void radio_button::on_key_press(key_event & e)
{
    if (e.code == key::enter ||
        (e.code == key::printable && e.character == U' ')) {
        set_checked(true);
        e.accept();
    }
}

void radio_button::on_mouse_press(mouse_event & e)
{
    set_checked(true);
    e.accept();
}

void radio_button::on_focus_in()  { update(); }
void radio_button::on_focus_out() { update(); }

} // namespace nx::tui
