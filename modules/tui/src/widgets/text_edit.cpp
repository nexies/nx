#include <nx/tui/widgets/text_edit.hpp>
#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/input/key_event.hpp>
#include <nx/tui/input/mouse_event.hpp>
#include <nx/tui/types/style_option.hpp>
#include <nx/tui/types/theme_role.hpp>
#include <nx/tui/animation/easing.hpp>

#include <nx/common/types/utf8.hpp>

#include <algorithm>

namespace nx::tui {

// ── UTF-8 helpers (file-local) ────────────────────────────────────────────────

static std::size_t
utf8_advance(const std::string & s, std::size_t start, std::size_t n)
{
    const char * p   = s.data() + start;
    const char * end = s.data() + s.size();
    for (std::size_t i = 0; i < n && p < end; ++i) {
        auto r = nx::utf8::detail::cluster_end(p, end);
        p = r ? *r : p + 1;
    }
    return static_cast<std::size_t>(p - s.data());
}

static std::size_t
utf8_col_count(const std::string & s, std::size_t from, std::size_t to)
{
    const char * p   = s.data() + from;
    const char * end = s.data() + to;
    std::size_t n    = 0;
    while (p < end) {
        auto r = nx::utf8::detail::cluster_end(p, end);
        p = r ? *r : p + 1;
        ++n;
    }
    return n;
}

static std::size_t
utf8_cluster_len(const std::string & s, std::size_t pos)
{
    const char * p   = s.data() + pos;
    const char * end = s.data() + s.size();
    auto r = nx::utf8::detail::cluster_end(p, end);
    return r ? static_cast<std::size_t>(*r - p) : 1;
}

static std::size_t
utf8_prev_cluster(const std::string & s, std::size_t pos)
{
    if (pos == 0) return 0;
    std::size_t p = pos - 1;
    while (p > 0 && (static_cast<unsigned char>(s[p]) & 0xC0) == 0x80) --p;
    return p;
}

// ── construction ──────────────────────────────────────────────────────────────

text_edit::text_edit(nx::core::object * parent)
    : widget(parent)
{
    set_focus_policy(focus_policy::tab_focus);
    // Flexible size — caller should set fixed dimensions or use in a layout.
    set_vertical_policy(size_policy::expanding);
    set_horizontal_policy(size_policy::expanding);
}

// ── Content ───────────────────────────────────────────────────────────────────

void text_edit::set_text(std::string t)
{
    lines_.clear();
    std::string::size_type start = 0, pos;
    while ((pos = t.find('\n', start)) != std::string::npos) {
        lines_.push_back(t.substr(start, pos - start));
        start = pos + 1;
    }
    lines_.push_back(t.substr(start));
    if (lines_.empty()) lines_.push_back("");

    cursor_row_  = 0;
    cursor_byte_ = 0;
    scroll_row_  = 0;
    scroll_col_  = 0;
    prefer_col_set_ = false;
    NX_EMIT(text_changed)
    update();
}

std::string text_edit::text() const
{
    std::string result;
    for (std::size_t i = 0; i < lines_.size(); ++i) {
        if (i) result += '\n';
        result += lines_[i];
    }
    return result;
}

int text_edit::cursor_col() const noexcept
{
    return (int)utf8_col_count(lines_[cursor_row_], 0, cursor_byte_);
}

widget::size_type text_edit::size_hint() const
{
    const auto h = explicit_hint();
    const auto s = size();
    return {
        h.height > 0 ? h.height : s.height,
        h.width  > 0 ? h.width  : s.width
    };
}

// ── Painting ──────────────────────────────────────────────────────────────────

void text_edit::on_paint(painter & p)
{
    const color fg_c = p.theme_color(theme_role::foreground);
    const color bg_c = p.theme_bg(theme_role::control);
    p.apply_style(fg(fg_c) | bg(bg_c));
    p.fill();

    const int h = size().height;
    const int w = size().width;
    if (h <= 0 || w <= 0) return;

    const float alpha = cursor_alpha_.value();

    for (int row = 0; row < h; ++row) {
        const int li = scroll_row_ + row;
        if (li >= (int)lines_.size()) break;
        const auto & line = lines_[(std::size_t)li];

        const std::size_t sb = utf8_advance(line, 0, (std::size_t)scroll_col_);
        const std::size_t eb = utf8_advance(line, sb, (std::size_t)w);

        if (sb < line.size())
            p.draw_text({ 0, row }, line.substr(sb, eb - sb));

        // Cursor on this row
        if (!has_focus() || li != (int)cursor_row_ || alpha <= 0.01f) continue;

        const std::size_t cur_vis = utf8_col_count(line, 0, cursor_byte_);
        const int scr_col = (int)cur_vis - scroll_col_;
        if (scr_col < 0 || scr_col >= w) continue;

        std::string cur_ch = " ";
        if (cursor_byte_ < line.size()) {
            const std::size_t clen = utf8_cluster_len(line, cursor_byte_);
            cur_ch = line.substr(cursor_byte_, clen);
        }
        p.with(fg(color::interpolate(alpha, fg_c, bg_c)) |
               bg(color::interpolate(alpha, bg_c, fg_c)))
         .draw_char({ scr_col, row }, cur_ch);
    }
}

// ── Keyboard ──────────────────────────────────────────────────────────────────

void text_edit::on_key_press(key_event & e)
{
    // Most movements reset the preferred column; vertical movement preserves it.
    bool preserve_col = false;

    switch (e.code) {
    case key::printable:
        if (e.modifiers.has(key_modifier::ctrl) || e.modifiers.has(key_modifier::alt))
            return;
        _insert_char(e.character);
        break;
    case key::enter:
        _insert_newline();
        break;
    case key::backspace:
        _delete_backward();
        break;
    case key::delete_key:
        _delete_forward();
        break;
    case key::arrow_left:
        _move_left();
        break;
    case key::arrow_right:
        _move_right();
        break;
    case key::arrow_up:
        _move_up();
        preserve_col = true;
        break;
    case key::arrow_down:
        _move_down();
        preserve_col = true;
        break;
    case key::home:
        _move_home();
        break;
    case key::end:
        _move_end();
        break;
    case key::page_up:
        _move_page_up();
        preserve_col = true;
        break;
    case key::page_down:
        _move_page_down();
        preserve_col = true;
        break;
    default:
        return;
    }

    if (!preserve_col) {
        preferred_col_   = _cursor_vis_col();
        prefer_col_set_  = true;
    }

    _adjust_scroll();
    NX_EMIT(cursor_moved, (int)cursor_row_, cursor_col())
    update();
    e.accept();
}

// ── Mouse ─────────────────────────────────────────────────────────────────────

void text_edit::on_mouse_press(mouse_event & e)
{
    if (e.button != mouse_button::left) return;

    const int target_row = std::clamp(scroll_row_ + e.position.y, 0, (int)lines_.size() - 1);
    cursor_row_ = (std::size_t)target_row;

    const auto & line   = lines_[cursor_row_];
    const int    target_col = scroll_col_ + e.position.x;
    cursor_byte_ = utf8_advance(line, 0, (std::size_t)std::max(0, target_col));
    cursor_byte_ = std::min(cursor_byte_, line.size());

    preferred_col_  = _cursor_vis_col();
    prefer_col_set_ = true;

    _adjust_scroll();
    NX_EMIT(cursor_moved, (int)cursor_row_, cursor_col())
    update();
    e.accept();
}

// ── Focus & blink ─────────────────────────────────────────────────────────────

void text_edit::on_focus_in()
{
    widget::on_focus_in();
    _blink_next();
    update();
}

void text_edit::on_focus_out()
{
    widget::on_focus_out();
    nx::core::disconnect(&cursor_alpha_.raw(), &animator::finished, this, nullptr);
    cursor_alpha_.set(0.0f);
    update();
}

void text_edit::_blink_next()
{
    if (!has_focus()) return;
    const float target = cursor_alpha_.value() > 0.5f ? 0.0f : 1.0f;
    cursor_alpha_.animate_to(target, 500, easing::ease_in_out);
    nx::core::connect(&cursor_alpha_.raw(), &animator::finished, this,
                      [this]() { _blink_next(); },
                      nx::core::connection_type::auto_t,
                      nx::core::connection_flag::single_shot);
}

// ── Cursor movement ───────────────────────────────────────────────────────────

void text_edit::_move_left()
{
    if (cursor_byte_ > 0) {
        cursor_byte_ = utf8_prev_cluster(lines_[cursor_row_], cursor_byte_);
    } else if (cursor_row_ > 0) {
        --cursor_row_;
        cursor_byte_ = lines_[cursor_row_].size();
    }
}

void text_edit::_move_right()
{
    const auto & line = lines_[cursor_row_];
    if (cursor_byte_ < line.size()) {
        cursor_byte_ = utf8_advance(line, cursor_byte_, 1);
    } else if (cursor_row_ + 1 < lines_.size()) {
        ++cursor_row_;
        cursor_byte_ = 0;
    }
}

std::size_t text_edit::_cursor_vis_col() const
{
    return utf8_col_count(lines_[cursor_row_], 0, cursor_byte_);
}

void text_edit::_move_up()
{
    if (cursor_row_ == 0) { cursor_byte_ = 0; return; }
    const std::size_t col = prefer_col_set_ ? preferred_col_ : _cursor_vis_col();
    --cursor_row_;
    const auto & line = lines_[cursor_row_];
    cursor_byte_    = utf8_advance(line, 0, col);
    cursor_byte_    = std::min(cursor_byte_, line.size());
}

void text_edit::_move_down()
{
    if (cursor_row_ + 1 >= lines_.size()) {
        cursor_byte_ = lines_[cursor_row_].size();
        return;
    }
    const std::size_t col = prefer_col_set_ ? preferred_col_ : _cursor_vis_col();
    ++cursor_row_;
    const auto & line = lines_[cursor_row_];
    cursor_byte_    = utf8_advance(line, 0, col);
    cursor_byte_    = std::min(cursor_byte_, line.size());
}

void text_edit::_move_home() { cursor_byte_ = 0; }
void text_edit::_move_end()  { cursor_byte_ = lines_[cursor_row_].size(); }

void text_edit::_move_page_up()
{
    const int page = std::max(1, size().height);
    const std::size_t col = prefer_col_set_ ? preferred_col_ : _cursor_vis_col();
    cursor_row_  = (std::size_t)std::max(0, (int)cursor_row_ - page);
    const auto & line = lines_[cursor_row_];
    cursor_byte_ = utf8_advance(line, 0, col);
    cursor_byte_ = std::min(cursor_byte_, line.size());
}

void text_edit::_move_page_down()
{
    const int page = std::max(1, size().height);
    const std::size_t col = prefer_col_set_ ? preferred_col_ : _cursor_vis_col();
    cursor_row_  = std::min(lines_.size() - 1,
                            cursor_row_ + (std::size_t)page);
    const auto & line = lines_[cursor_row_];
    cursor_byte_ = utf8_advance(line, 0, col);
    cursor_byte_ = std::min(cursor_byte_, line.size());
}

// ── Edit operations ───────────────────────────────────────────────────────────

void text_edit::_insert_char(char32_t cp)
{
    char buf[5] = {};
    std::size_t clen;
    if      (cp < 0x80)    { buf[0] = char(cp);                                              clen = 1; }
    else if (cp < 0x800)   { buf[0] = char(0xC0|(cp>>6));  buf[1] = char(0x80|(cp&0x3F));   clen = 2; }
    else if (cp < 0x10000) { buf[0] = char(0xE0|(cp>>12)); buf[1] = char(0x80|((cp>>6)&0x3F));
                             buf[2] = char(0x80|(cp&0x3F));                                   clen = 3; }
    else                   { buf[0] = char(0xF0|(cp>>18)); buf[1] = char(0x80|((cp>>12)&0x3F));
                             buf[2] = char(0x80|((cp>>6)&0x3F)); buf[3] = char(0x80|(cp&0x3F)); clen = 4; }

    lines_[cursor_row_].insert(cursor_byte_, buf, clen);
    cursor_byte_ += clen;
    NX_EMIT(text_changed)
}

void text_edit::_insert_newline()
{
    auto & cur = lines_[cursor_row_];
    std::string tail = cur.substr(cursor_byte_);
    cur.erase(cursor_byte_);
    lines_.insert(lines_.begin() + (std::ptrdiff_t)cursor_row_ + 1, std::move(tail));
    ++cursor_row_;
    cursor_byte_ = 0;
    NX_EMIT(text_changed)
}

void text_edit::_delete_backward()
{
    if (cursor_byte_ > 0) {
        auto & line = lines_[cursor_row_];
        const std::size_t prev = utf8_prev_cluster(line, cursor_byte_);
        line.erase(prev, cursor_byte_ - prev);
        cursor_byte_ = prev;
        NX_EMIT(text_changed)
    } else if (cursor_row_ > 0) {
        const std::size_t prev_len = lines_[cursor_row_ - 1].size();
        lines_[cursor_row_ - 1] += lines_[cursor_row_];
        lines_.erase(lines_.begin() + (std::ptrdiff_t)cursor_row_);
        --cursor_row_;
        cursor_byte_ = prev_len;
        NX_EMIT(text_changed)
    }
}

void text_edit::_delete_forward()
{
    auto & line = lines_[cursor_row_];
    if (cursor_byte_ < line.size()) {
        const std::size_t clen = utf8_cluster_len(line, cursor_byte_);
        line.erase(cursor_byte_, clen);
        NX_EMIT(text_changed)
    } else if (cursor_row_ + 1 < lines_.size()) {
        line += lines_[cursor_row_ + 1];
        lines_.erase(lines_.begin() + (std::ptrdiff_t)cursor_row_ + 1);
        NX_EMIT(text_changed)
    }
}

// ── Scroll adjustment ─────────────────────────────────────────────────────────

void text_edit::_adjust_scroll()
{
    const int h = size().height;
    const int w = size().width;
    if (h <= 0 || w <= 0) return;

    // Vertical
    const int cr = (int)cursor_row_;
    if (cr < scroll_row_)
        scroll_row_ = cr;
    else if (cr >= scroll_row_ + h)
        scroll_row_ = cr - h + 1;

    // Horizontal
    const int cc = (int)_cursor_vis_col();
    if (cc < scroll_col_)
        scroll_col_ = cc;
    else if (cc >= scroll_col_ + w)
        scroll_col_ = cc - w + 1;
}

} // namespace nx::tui
