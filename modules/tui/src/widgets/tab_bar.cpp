#include <nx/tui/widgets/tab_bar.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/input/key.hpp>
#include <nx/common/types/utf8.hpp>

#include <algorithm>

namespace nx::tui {

tab_bar::tab_bar(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
    set_fixed_height(1);
}

// ── geometry helpers ──────────────────────────────────────────────────────────

int tab_bar::_tab_w(int idx) const noexcept
{
    if (idx < 0 || idx >= static_cast<int>(tabs_.size())) return 0;
    // Count grapheme clusters so that multi-byte labels work correctly.
    int glyphs = 0;
    for (auto it = nx::utf8::view(tabs_[idx]).begin(),
              end = nx::utf8::view(tabs_[idx]).end(); it != end; ++it) {
        if (!*it) break;
        ++glyphs;
    }
    return glyphs + 2; // 1 padding each side
}

int tab_bar::_tab_x(int idx) const noexcept
{
    int x = 0;
    for (int i = 0; i < idx; ++i)
        x += _tab_w(i);
    return x;
}

int tab_bar::_tab_at_col(int col) const noexcept
{
    int x = 0;
    for (int i = 0; i < static_cast<int>(tabs_.size()); ++i) {
        const int w = _tab_w(i);
        if (col >= x && col < x + w) return i;
        x += w;
    }
    return -1;
}

// ── indicator helpers ─────────────────────────────────────────────────────────

void tab_bar::_snap_indicator() noexcept
{
    if (current_ < 0 || current_ >= static_cast<int>(tabs_.size())) return;
    ind_x_.set(_tab_x(current_));
    ind_w_.set(_tab_w(current_));
}

void tab_bar::_animate_indicator() noexcept
{
    if (current_ < 0 || current_ >= static_cast<int>(tabs_.size())) return;
    ind_x_.animate_to(_tab_x(current_), 150, easing::ease_out);
    ind_w_.animate_to(_tab_w(current_), 120, easing::ease_out);
}

// ── tab management ────────────────────────────────────────────────────────────

void tab_bar::add_tab(std::string title)
{
    tabs_.push_back(std::move(title));
    if (current_ < 0) {
        current_ = 0;
        _snap_indicator();
    }
    update();
}

void tab_bar::remove_tab(int idx)
{
    if (idx < 0 || idx >= static_cast<int>(tabs_.size())) return;
    tabs_.erase(tabs_.begin() + idx);
    if (tabs_.empty()) {
        current_ = -1;
    } else {
        current_ = std::clamp(current_, 0, static_cast<int>(tabs_.size()) - 1);
        _snap_indicator();
    }
    update();
}

void tab_bar::set_tab_title(int idx, std::string title)
{
    if (idx < 0 || idx >= static_cast<int>(tabs_.size())) return;
    tabs_[idx] = std::move(title);
    if (idx == current_) _snap_indicator();
    update();
}

const std::string & tab_bar::tab_title(int idx) const
{
    static const std::string empty;
    if (idx < 0 || idx >= static_cast<int>(tabs_.size())) return empty;
    return tabs_[idx];
}

void tab_bar::set_current(int idx)
{
    if (tabs_.empty()) return;
    idx = std::clamp(idx, 0, static_cast<int>(tabs_.size()) - 1);
    if (idx == current_) return;
    current_ = idx;
    _animate_indicator();
    update();
    NX_EMIT(current_changed, current_)
}

// ── size hint ─────────────────────────────────────────────────────────────────

widget::size_type tab_bar::size_hint() const
{
    int total_w = 0;
    for (int i = 0; i < static_cast<int>(tabs_.size()); ++i)
        total_w += _tab_w(i);
    return { 1, total_w };
}

// ── painting ──────────────────────────────────────────────────────────────────

void tab_bar::on_paint(painter & p)
{
    p.apply_theme_as_base(theme_role::foreground, theme_role::background);
    p.fill(" ");

    if (tabs_.empty()) return;

    const int ul_x = ind_x_.value();
    const int ul_w = ind_w_.value();

    const bool focused  = has_focus();
    const color ul_color  = focused ? p.theme_color(theme_role::border_focus)
                                    : p.theme_color(theme_role::accent);
    const color dim_color = p.theme_color(theme_role::foreground_dim);
    const color fg_color  = p.theme_color(theme_role::foreground);

    for (int i = 0; i < static_cast<int>(tabs_.size()); ++i) {
        const int tx = _tab_x(i);
        const std::string & name = tabs_[i];

        // Build grapheme cluster list for this label.
        std::vector<std::string> glyphs;
        glyphs.push_back(" "); // leading padding
        for (auto it = nx::utf8::view(name).begin(),
                  end = nx::utf8::view(name).end(); it != end; ++it) {
            auto g = *it;
            if (!g) break;
            glyphs.push_back(std::string(g->bytes()));
        }
        glyphs.push_back(" "); // trailing padding

        for (int c = 0; c < static_cast<int>(glyphs.size()); ++c) {
            const int col = tx + c;
            const bool underlined = (col >= ul_x && col < ul_x + ul_w);

            const color fg = underlined ? ul_color
                           : (i == current_ ? fg_color : dim_color);

            p.set_color(fg);
            p.set_style(underlined ? pixel_style_flag::underline
                                   : pixel_style_flag::none);
            p.draw_char({ col, 0 }, glyphs[c]);
        }
    }

    // Reset style to avoid leaking into subsequent draws by the same painter.
    p.set_style(pixel_style_flag::none);
    p.set_color(fg_color);
}

// ── input ─────────────────────────────────────────────────────────────────────

void tab_bar::on_mouse_press(mouse_event & e)
{
    const int idx = _tab_at_col(e.position.x);
    if (idx >= 0)
        set_current(idx);
}

void tab_bar::on_key_press(key_event & e)
{
    if (current_ < 0) return;
    if (e.code == key::arrow_left || e.code == key::arrow_up) {
        if (current_ > 0) set_current(current_ - 1);
        e.accept();
    } else if (e.code == key::arrow_right || e.code == key::arrow_down) {
        if (current_ < static_cast<int>(tabs_.size()) - 1)
            set_current(current_ + 1);
        e.accept();
    }
}

void tab_bar::on_focus_in()  { update(); }
void tab_bar::on_focus_out() { update(); }

} // namespace nx::tui
