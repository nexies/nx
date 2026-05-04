#include <nx/tui/graphics/painter.hpp>
#include <nx/tui/application.hpp>
#include <nx/common/types/utf8.hpp>

#include <algorithm>

namespace nx::tui {

// ── border helpers ────────────────────────────────────────────────────────────

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

// ── construction ──────────────────────────────────────────────────────────────

painter::painter(buffer_type & buffer)
    : buffer_(buffer)
    , rect_(buffer.rect())
    , clip_(buffer.rect())
{}

painter::painter(buffer_type & buffer, rect_type render_rect)
    : buffer_(buffer)
    , rect_(render_rect)
    , clip_(render_rect)
{}

painter::painter(buffer_type & buffer, rect_type render_rect, rect_type clip_rect)
    : buffer_(buffer)
    , rect_(render_rect)
    , clip_(clip_rect)
{}

// ── static style ──────────────────────────────────────────────────────────────

void painter::enable_style(pixel_style s) noexcept
{
    style_.decorations = style_.decorations.value_or(pixel_style_flag::none) | s;
}

void painter::disable_style(pixel_style s) noexcept
{
    style_.decorations = style_.decorations.value_or(pixel_style_flag::none) & ~s;
}

void painter::set_style(pixel_style s) noexcept
{
    style_.decorations = s;
}

void painter::set_color(const color & c) noexcept
{
    style_.foreground = c;
}

void painter::set_background_color(const color & c) noexcept
{
    style_.background = c;
}

void painter::apply_style(const style_option & s) noexcept
{
    style_ |= s;
}

painter painter::with(const style_option & s) const
{
    painter copy = *this;
    copy.style_ |= s;
    return copy;
}

// ── per-cell helpers ──────────────────────────────────────────────────────────

style_option painter::_at(int col, int row) const noexcept
{
    // Start with static fields only (no modifiers — avoids re-entering the chain).
    style_option base;
    base.foreground  = style_.foreground;
    base.background  = style_.background;
    base.decorations = style_.decorations;

    // Run modifier chain in order.
    for (const auto & mod : style_.modifiers)
        if (mod) base = mod->transform(base, col, row, rect_.width(), rect_.height());

    return base;
}

void painter::_write(int bx, int by, int lc, int lr, const std::string & ch) const
{
    if (bx < clip_.x() || bx >= clip_.x() + clip_.width())  return;
    if (by < clip_.y() || by >= clip_.y() + clip_.height()) return;

    auto   s  = _at(lc, lr);
    auto & px = buffer_.pixel_at(bx, by);

    color new_bg;
    if (s.background.has_value()) {
        new_bg = s.background.value();
    } else {
        new_bg = style_.background.value_or(color::default_color);
    }

    // Alpha blend: true-color backgrounds with non-zero alpha are semi-transparent.
    // (alpha=0 → fully opaque, alpha=255 → fully transparent, values between → blend)
    if (new_bg != color::default_color &&
        new_bg.type() == color_type::true_color &&
        new_bg.a() > 0 && new_bg.a() < 255) {
        new_bg = color::blend(new_bg, px.background_color);
    }

    px.character        = ch;
    px.foreground_color = s.foreground.value_or(color::default_color);
    px.background_color = new_bg;
    px.style            = s.decorations.value_or(pixel_style_flag::none);
}

// ── draw operations ───────────────────────────────────────────────────────────

void painter::draw_text(const point_type & pos, const std::string & text) const
{
    const int base_x = rect_.x() + pos.x;
    const int by     = rect_.y() + pos.y;

    if (by < rect_.y() || by >= rect_.y() + rect_.height()) return;

    const int local_row = pos.y;
    int       col       = 0;

    for (auto && it : nx::utf8::view(text)) {
        const int bx = base_x + col;
        if (bx >= rect_.x() + rect_.width()) break;

        auto g = it;
        if (!g) break; // invalid UTF-8 — stop

        if (bx >= rect_.x())
            _write(bx, by, pos.x + col, local_row, std::string(g->bytes()));

        ++col;
    }
}

void painter::draw_char(const point_type & pos, const std::string & ch) const
{
    draw_text(pos, ch);
}

void painter::fill(const std::string & ch) const
{
    // Iterate over the intersection of render_rect and clip_ to avoid
    // walking cells that would be rejected by _write's bounds check.
    const int bx0 = std::max(rect_.x(), clip_.x());
    const int by0 = std::max(rect_.y(), clip_.y());
    const int bx1 = std::min(rect_.x() + rect_.width(),  clip_.x() + clip_.width());
    const int by1 = std::min(rect_.y() + rect_.height(), clip_.y() + clip_.height());

    for (int by = by0; by < by1; ++by) {
        for (int bx = bx0; bx < bx1; ++bx) {
            _write(bx, by, bx - rect_.x(), by - rect_.y(), ch);
        }
    }
}

void painter::clear(bool keep_background) const
{
    const int bx0 = std::max(rect_.x(), clip_.x());
    const int by0 = std::max(rect_.y(), clip_.y());
    const int bx1 = std::min(rect_.x() + rect_.width(),  clip_.x() + clip_.width());
    const int by1 = std::min(rect_.y() + rect_.height(), clip_.y() + clip_.height());

    if (!keep_background) {
        // Standard clear: write space with the current background color.
        fill(" ");
    } else {
        // Erase characters only — preserve each cell's existing background_color.
        // Useful in transparent overlay widgets that want to clear stale text
        // without overwriting the background painted by a lower layer.
        auto s = _at(0, 0); // sample effective foreground / decorations
        for (int by = by0; by < by1; ++by) {
            for (int bx = bx0; bx < bx1; ++bx) {
                auto & px = buffer_.pixel_at(bx, by);
                px.character        = " ";
                px.foreground_color = s.foreground.value_or(color::default_color);
                px.style            = s.decorations.value_or(pixel_style_flag::none);
                // background_color intentionally left unchanged
            }
        }
    }
}

painter::point_type painter::_project_point(const point_type & pos) const
{
    return { pos.x + rect_.x(), pos.y + rect_.y() };
}

// ── theme access ──────────────────────────────────────────────────────────────

const style_option & painter::theme_style(theme_role r) const noexcept
{
    static const style_option empty;
    if (auto * app = tui_application::instance())
        return app->get_theme().get_style(r);
    return empty;
}

color painter::theme_color(theme_role r) const noexcept
{
    if (auto * app = tui_application::instance())
        return app->get_theme().get_color(r);
    return color::default_color;
}

color painter::theme_bg(theme_role r) const noexcept
{
    if (auto * app = tui_application::instance())
        return app->get_theme().get_bg(r);
    return color::default_color;
}

void painter::apply_theme_as_base(theme_role fg_role, theme_role bg_role) noexcept
{
    // Only fills in fields not already set by an explicit widget style.
    if (!style_.foreground) {
        const color c = theme_color(fg_role);
        if (c != color::default_color) style_.foreground = c;
    }
    if (!style_.background) {
        const color c = theme_bg(bg_role);
        if (c != color::default_color) style_.background = c;
    }
}

// ── geometry primitives ───────────────────────────────────────────────────────

void painter::draw_text_aligned(const std::string & text, text_align align, int row) const
{
    const int w = rect_.width();
    if (w <= 0) return;

    // Count grapheme clusters and find the byte boundary at `w`.
    int   cols     = 0;
    const char * end_byte = text.data();
    for (auto it = nx::utf8::view(text).begin(), e = nx::utf8::view(text).end();
         it != e; ++it) {
        if (cols >= w) break;
        auto g = *it;
        if (!g) break;
        end_byte = g->bytes().data() + g->bytes().size();
        ++cols;
    }

    const std::string visible(text.data(), static_cast<std::size_t>(end_byte - text.data()));

    int x = 0;
    switch (align) {
    case text_align::left:   x = 0;                    break;
    case text_align::center: x = (w - cols) / 2;       break;
    case text_align::right:  x = w - cols;              break;
    }
    if (x < 0) x = 0;

    draw_text({ x, row }, visible);
}

void painter::draw_hline(int row, const std::string & ch) const
{
    const int w = rect_.width();
    for (int x = 0; x < w; ++x)
        draw_char({ x, row }, ch);
}

void painter::draw_vline(int col, const std::string & ch) const
{
    const int h = rect_.height();
    for (int y = 0; y < h; ++y)
        draw_char({ col, y }, ch);
}

void painter::draw_border(border_style bs) const
{
    if (bs == border_style::none) return;
    const int w = rect_.width();
    const int h = rect_.height();
    if (w < 2 || h < 2) return;

    const auto bc = chars_for(bs);

    draw_char({ 0,     0     }, bc.tl);
    draw_char({ w - 1, 0     }, bc.tr);
    draw_char({ 0,     h - 1 }, bc.bl);
    draw_char({ w - 1, h - 1 }, bc.br);

    for (int col = 1; col < w - 1; ++col) {
        draw_char({ col, 0     }, bc.h);
        draw_char({ col, h - 1 }, bc.h);
    }
    for (int row = 1; row < h - 1; ++row) {
        draw_char({ 0,     row }, bc.v);
        draw_char({ w - 1, row }, bc.v);
    }
}

void painter::draw_border(border_style bs,
                          const std::string & title,
                          const style_option & title_style) const
{
    draw_border(bs);

    if (title.empty() || bs == border_style::none) return;

    const int w = rect_.width();
    if (w < 6) return;

    const auto bc = chars_for(bs);
    const std::string padded = " " + title + " ";
    const int max_cols = w - 4;

    // Truncate title to fit.
    int   cols     = 0;
    const char * end_byte = padded.data();
    for (auto it = nx::utf8::view(padded).begin(), e = nx::utf8::view(padded).end();
         it != e; ++it) {
        if (cols >= max_cols) break;
        auto g = *it;
        if (!g) break;
        end_byte = g->bytes().data() + g->bytes().size();
        ++cols;
    }
    const std::string rendered(padded.data(), static_cast<std::size_t>(end_byte - padded.data()));

    // Overwrite the top-edge h-chars at col 1 and col 2+(title_len) with border chars
    // to close the gap cleanly, then draw the title text.
    draw_char({ 1, 0 }, bc.h);
    const int end_col = 2 + cols;
    if (end_col < w - 1)
        draw_char({ end_col, 0 }, bc.h);

    with(title_style).draw_text({ 2, 0 }, rendered);
}

} // namespace nx::tui
