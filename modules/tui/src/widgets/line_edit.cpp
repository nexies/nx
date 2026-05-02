#include <nx/tui/widgets/line_edit.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/animation/easing.hpp>

#include <nx/common/types/utf8.hpp>

namespace nx::tui {

// ── UTF-8 / grapheme helpers (file-local) ─────────────────────────────────────

// Advance `n` grapheme clusters forward from byte offset `start`.
// Returns the resulting byte offset.
static std::size_t
utf8_advance(const std::string & text, std::size_t start, std::size_t n)
{
    const char * p   = text.data() + start;
    const char * end = text.data() + text.size();
    for (std::size_t i = 0; i < n && p < end; ++i) {
        auto r = nx::utf8::detail::cluster_end(p, end);
        p = r ? *r : p + 1; // skip one bad byte on error
    }
    return static_cast<std::size_t>(p - text.data());
}

// Count grapheme clusters in [from_byte, to_byte).
static std::size_t
utf8_col_count(const std::string & text, std::size_t from_byte, std::size_t to_byte)
{
    const char * p   = text.data() + from_byte;
    const char * end = text.data() + to_byte;
    std::size_t count = 0;
    while (p < end) {
        auto r = nx::utf8::detail::cluster_end(p, end);
        p = r ? *r : p + 1;
        ++count;
    }
    return count;
}

// Return the byte length of the grapheme cluster starting at `pos`.
static std::size_t
utf8_cluster_len(const std::string & text, std::size_t pos)
{
    const char * p   = text.data() + pos;
    const char * end = text.data() + text.size();
    auto r = nx::utf8::detail::cluster_end(p, end);
    return r ? static_cast<std::size_t>(*r - p) : 1;
}

// Find the start of the grapheme cluster that ends at byte offset `pos`.
// Steps back over UTF-8 continuation bytes (codepoint boundary), then
// checks whether combining marks attach it to an earlier base.
static std::size_t
utf8_prev_cluster_start(const std::string & text, std::size_t pos)
{
    if (pos == 0) return 0;
    // Step back to the start of the previous codepoint.
    std::size_t p = pos - 1;
    while (p > 0 && (static_cast<unsigned char>(text[p]) & 0xC0) == 0x80) --p;
    return p;
}



line_edit::line_edit(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
    set_fixed_height(1);
}

widget::size_type line_edit::size_hint() const
{
    const int w = explicit_hint().width > 0 ? explicit_hint().width : size().width;
    return { 1, w };
}

void line_edit::set_text(std::string t)
{
    text_       = std::move(t);
    cursor_     = text_.size();
    scroll_off_ = 0;
    _adjust_scroll();
    NX_EMIT(text_changed)
    update();
}

// ── painting ──────────────────────────────────────────────────────────────────

void line_edit::on_paint(painter & p)
{
    // Apply theme defaults for fg/bg unless the widget has an explicit style.
    p.apply_theme_as_base(theme_role::foreground, theme_role::background);

    p.fill(" ");

    const int w = size().width;
    if (w <= 0) return;

    // scroll_off_ and cursor_ are byte offsets; view width is in columns.
    const std::size_t view_cols = static_cast<std::size_t>(w);
    const std::size_t start     = scroll_off_;
    const std::size_t end       = utf8_advance(text_, start, view_cols);

    if (start < text_.size())
        p.draw_text({ 0, 0 }, text_.substr(start, end - start));

    // Draw cursor when focused: smooth fade between normal and inverted using
    // cursor_alpha_ (0 = invisible, 1 = fully inverted).
    if (has_focus()) {
        const int cur_col = static_cast<int>(utf8_col_count(text_, scroll_off_, cursor_));
        if (cur_col >= 0 && cur_col < w) {
            const float alpha = cursor_alpha_.value();
            if (alpha > 0.01f) {
                std::string cur_ch = " ";
                if (cursor_ < text_.size()) {
                    const std::size_t clen = utf8_cluster_len(text_, cursor_);
                    cur_ch = std::string(text_.data() + cursor_, clen);
                }

                // fg/bg from painter (already resolved to theme defaults).
                color fg_c = p.current_color();
                color bg_c = p.current_background_color();
                // Last-resort fallbacks when no app/theme available.
                if (fg_c == color::default_color)
                    fg_c = p.theme_color(theme_role::foreground);
                if (bg_c == color::default_color)
                    bg_c = p.theme_bg(theme_role::background);

                const color c_fg = color::interpolate(alpha, fg_c, bg_c);
                const color c_bg = color::interpolate(alpha, bg_c, fg_c);

                painter cur_p = p;
                cur_p.apply_style(fg(c_fg) | bg(c_bg));
                cur_p.draw_char({ cur_col, 0 }, cur_ch);
            }
        }
    }
}

// ── keyboard ──────────────────────────────────────────────────────────────────

void line_edit::on_key_press(key_event & e)
{
    // When set, _adjust_scroll will lock the cursor to this visual column
    // instead of just ensuring it is visible.
    std::optional<std::size_t> pin_vis_col;

    switch (e.code) {
    case key::printable: {
        // Don't insert text for modified characters (Ctrl+X, Alt+X are commands).
        if (e.modifiers.has(key_modifier::ctrl) || e.modifiers.has(key_modifier::alt))
            break;

        // Encode the Unicode codepoint as UTF-8 and insert at cursor.
        char buf[5] = {};
        std::size_t clen;
        const char32_t cp = e.character;
        if (cp < 0x80) {
            buf[0] = static_cast<char>(cp);
            clen   = 1;
        } else if (cp < 0x800) {
            buf[0] = static_cast<char>(0xC0 | (cp >> 6));
            buf[1] = static_cast<char>(0x80 | (cp & 0x3F));
            clen   = 2;
        } else if (cp < 0x10000) {
            buf[0] = static_cast<char>(0xE0 | (cp >> 12));
            buf[1] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            buf[2] = static_cast<char>(0x80 | (cp & 0x3F));
            clen   = 3;
        } else {
            buf[0] = static_cast<char>(0xF0 | (cp >> 18));
            buf[1] = static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            buf[2] = static_cast<char>(0x80 | ((cp >> 6)  & 0x3F));
            buf[3] = static_cast<char>(0x80 | (cp & 0x3F));
            clen   = 4;
        }
        text_.insert(cursor_, buf, clen);
        cursor_ += clen;  // advance by bytes, not by 1
        NX_EMIT(text_changed)
        break;
    }
    case key::backspace:
        if (cursor_ > 0) {
            // When the view is scrolled, keep the cursor at the same screen
            // column after deletion (content to the left shifts left).
            if (scroll_off_ > 0)
                pin_vis_col = utf8_col_count(text_, scroll_off_, cursor_);

            const std::size_t pos = utf8_prev_cluster_start(text_, cursor_);
            text_.erase(pos, cursor_ - pos);
            cursor_ = pos;
            NX_EMIT(text_changed)
        }
        break;
    case key::delete_key:
        if (cursor_ < text_.size()) {
            // Cursor does not move on Delete, so its visual column is preserved
            // automatically — no pin needed (symmetric with Backspace).
            const std::size_t clen = utf8_cluster_len(text_, cursor_);
            text_.erase(cursor_, clen);
            NX_EMIT(text_changed)
        }
        break;
    case key::arrow_left:
        if (cursor_ > 0)
            cursor_ = utf8_prev_cluster_start(text_, cursor_);
        break;
    case key::arrow_right:
        if (cursor_ < text_.size())
            cursor_ = utf8_advance(text_, cursor_, 1);
        break;
    case key::home:
        cursor_ = 0;
        break;
    case key::end:
        cursor_ = text_.size();
        break;
    case key::enter:
        NX_EMIT(return_pressed)
        break;
    default:
        return;
    }

    _adjust_scroll(pin_vis_col);
    update();
}

// ── mouse ─────────────────────────────────────────────────────────────────────

void line_edit::on_mouse_press(mouse_event & e)
{
    if (e.button != mouse_button::left) return;

    const int local_col = e.position.x;
    const std::size_t target =
        scroll_off_ + static_cast<std::size_t>(local_col < 0 ? 0 : local_col);
    cursor_ = std::min(target, text_.size());

    _adjust_scroll();
    update();
}

// ── Focus / blink ─────────────────────────────────────────────────────────────

void line_edit::on_focus_in()
{
    widget::on_focus_in();
    _blink_next();
    update();
}

void line_edit::on_focus_out()
{
    widget::on_focus_out();
    // Remove any pending blink callback and snap cursor to hidden.
    nx::core::disconnect(&cursor_alpha_.raw(), &animator::finished, this, nullptr);
    cursor_alpha_.set(0.0f);
    update();
}

void line_edit::_blink_next()
{
    if (!has_focus()) return;
    const float target = cursor_alpha_.value() > 0.5f ? 0.0f : 1.0f;
    cursor_alpha_.animate_to(target, 500, easing::ease_in_out);
    nx::core::connect(&cursor_alpha_.raw(), &animator::finished, this,
                      [this]() { _blink_next(); },
                      nx::core::connection_type::auto_t,
                      nx::core::connection_flag::single_shot);
}

// ── scroll adjustment ─────────────────────────────────────────────────────────

void line_edit::_adjust_scroll(std::optional<std::size_t> pin_vis_col)
{
    const int w = size().width;
    if (w <= 0) return;
    const std::size_t view_cols = static_cast<std::size_t>(w);

    // cursor_ and scroll_off_ are byte offsets; compare in visual columns.
    const std::size_t cursor_col = utf8_col_count(text_, 0, cursor_);

    if (pin_vis_col.has_value()) {
        // Pin mode — lock cursor to the requested screen column.
        // new_scroll_col = cursor_col - pin_vis_col  (guaranteed >= 0 because
        // cursor was visible before the operation that set the pin).
        const std::size_t vc = *pin_vis_col;
        if (cursor_col >= vc)
            scroll_off_ = utf8_advance(text_, 0, cursor_col - vc);
        else
            scroll_off_ = 0;
        return;
    }

    // Normal mode — ensure cursor stays within the visible window.
    // Each arrow keypress at the boundary scrolls by exactly one grapheme,
    // keeping the cursor at the edge while the content shifts.
    const std::size_t scroll_col = utf8_col_count(text_, 0, scroll_off_);
    if (cursor_col >= scroll_col + view_cols) {
        // Cursor is past the right edge — scroll right.
        scroll_off_ = utf8_advance(text_, 0, cursor_col - view_cols + 1);
    } else if (cursor_col < scroll_col) {
        // Cursor is before the left edge — scroll left.
        scroll_off_ = cursor_;
    }
}

} // namespace nx::tui
