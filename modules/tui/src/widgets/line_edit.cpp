#include <nx/tui/widgets/line_edit.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/detail/utf8.hpp>

namespace nx::tui {

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
    p.fill(" ");

    const int w = size().width;
    if (w <= 0) return;

    // scroll_off_ and cursor_ are byte offsets; view width is in columns.
    const std::size_t view_cols = static_cast<std::size_t>(w);
    const std::size_t start     = scroll_off_;
    const std::size_t end       = utf8::advance(text_, start, view_cols);

    if (start < text_.size()) {
        p.draw_text({ 0, 0 }, text_.substr(start, end - start));
    }

    // Draw cursor when focused (inverted cell at cursor column).
    if (has_focus()) {
        const int cur_col = static_cast<int>(utf8::col_count(text_, scroll_off_, cursor_));
        if (cur_col >= 0 && cur_col < w) {
            std::string cur_ch = " ";
            if (cursor_ < text_.size()) {
                std::size_t clen = utf8::char_len(text_, cursor_);
                if (cursor_ + clen > text_.size()) clen = text_.size() - cursor_;
                cur_ch = std::string(text_.data() + cursor_, clen);
            }
            painter cur_p = p;
            cur_p.apply_style(inverted());
            cur_p.draw_char({ cur_col, 0 }, cur_ch);
        }
    }
}

// ── keyboard ──────────────────────────────────────────────────────────────────

void line_edit::on_key_press(key_event & e)
{
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
            // Step back over a UTF-8 continuation sequence.
            std::size_t pos = cursor_ - 1;
            while (pos > 0 && (text_[pos] & 0xC0) == 0x80) --pos;
            text_.erase(pos, cursor_ - pos);
            cursor_ = pos;
            NX_EMIT(text_changed)
        }
        break;
    case key::delete_key:
        if (cursor_ < text_.size()) {
            std::size_t next = cursor_ + 1;
            while (next < text_.size() && (text_[next] & 0xC0) == 0x80) ++next;
            text_.erase(cursor_, next - cursor_);
            NX_EMIT(text_changed)
        }
        break;
    case key::arrow_left:
        if (cursor_ > 0) {
            --cursor_;
            while (cursor_ > 0 && (text_[cursor_] & 0xC0) == 0x80) --cursor_;
        }
        break;
    case key::arrow_right:
        if (cursor_ < text_.size()) {
            ++cursor_;
            while (cursor_ < text_.size() && (text_[cursor_] & 0xC0) == 0x80) ++cursor_;
        }
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

    _adjust_scroll();
    update();
}

// ── mouse ─────────────────────────────────────────────────────────────────────

void line_edit::on_mouse_press(mouse_event & e)
{
    if (e.button != mouse_button::left) return;

    const int local_col = e.position.x - 1;
    const std::size_t target =
        scroll_off_ + static_cast<std::size_t>(local_col < 0 ? 0 : local_col);
    cursor_ = std::min(target, text_.size());

    _adjust_scroll();
    update();
}

// ── scroll adjustment ─────────────────────────────────────────────────────────

void line_edit::_adjust_scroll()
{
    const int w = size().width;
    if (w <= 0) return;
    const std::size_t view_cols = static_cast<std::size_t>(w);

    // cursor_ and scroll_off_ are byte offsets; compare in visual columns.
    const std::size_t cursor_col = utf8::col_count(text_, 0, cursor_);
    const std::size_t scroll_col = utf8::col_count(text_, 0, scroll_off_);

    if (cursor_col >= scroll_col + view_cols) {
        // Cursor is past the right edge — scroll right.
        const std::size_t new_scroll_col = cursor_col - view_cols + 1;
        scroll_off_ = utf8::advance(text_, 0, new_scroll_col);
    } else if (cursor_col < scroll_col) {
        // Cursor is before the left edge — scroll left.
        scroll_off_ = cursor_;
    }
}

} // namespace nx::tui
