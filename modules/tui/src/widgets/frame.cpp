#include <nx/tui/widgets/frame.hpp>
#include <nx/tui/graphics/painter.hpp>

#include <algorithm>
#include <string>

namespace nx::tui {

// ── border character sets ─────────────────────────────────────────────────────

namespace {

struct border_chars {
    // horiz, vert, top_left, top_right, bot_left, bot_right
    const char * h;
    const char * v;
    const char * tl;
    const char * tr;
    const char * bl;
    const char * br;
};

border_chars chars_for(border_style s) noexcept
{
    switch (s) {
    case border_style::single:
        return { "─", "│", "┌", "┐", "└", "┘" };
    case border_style::double_:
        return { "═", "║", "╔", "╗", "╚", "╝" };
    case border_style::rounded:
        return { "─", "│", "╭", "╮", "╰", "╯" };
    case border_style::thick:
        return { "━", "┃", "┏", "┓", "┗", "┛" };
    case border_style::dashed:
        return { "╌", "╎", "┌", "┐", "└", "┘" };
    default:
        return { " ", " ", " ", " ", " ", " " };
    }
}

} // namespace

// ── frame ─────────────────────────────────────────────────────────────────────

frame::frame(nx::core::object * parent)
    : widget(parent)
{}

// Position all children inside the 1-cell border margin.
void frame::_apply_layout()
{
    const int pw = size().width;
    const int ph = size().height;
    if (pw < 3 || ph < 3) return;

    for (auto * child : child_widgets())
        child->set_geometry({1, 1, pw - 2, ph - 2});
}

widget::size_type frame::size_hint() const
{
    const auto h = explicit_hint();
    const auto s = size();
    return {
        (h.height > 0 ? h.height : s.height),
        (h.width  > 0 ? h.width  : s.width)
    };
}

void frame::on_paint(painter & p)
{
    if (border_style_ == border_style::none) {
        p.fill(" ");
        return;
    }

    const auto bc = chars_for(border_style_);
    const int  w  = size().width;
    const int  h  = size().height;

    if (w < 2 || h < 2) return;

    // Apply border color on top of whatever the painter already has.
    const color saved_color = p.current_color();
    if (border_color_ != color::default_color) {
        p.set_color(border_color_);
    }

    // Fill inner area (transparent frames skip this so lower layers show through).
    p.set_color(saved_color);
    if (!is_transparent()) {
        for (int row = 1; row < h - 1; ++row) {
            for (int col = 1; col < w - 1; ++col) {
                p.draw_char({ col, row }, " ");
            }
        }
    }

    // Now draw border in border color.
    if (border_color_ != color::default_color) {
        p.set_color(border_color_);
    }

    // Corners.
    p.draw_char({ 0,     0     }, bc.tl);
    p.draw_char({ w - 1, 0     }, bc.tr);
    p.draw_char({ 0,     h - 1 }, bc.bl);
    p.draw_char({ w - 1, h - 1 }, bc.br);

    // Top and bottom edges.
    for (int col = 1; col < w - 1; ++col) {
        p.draw_char({ col, 0     }, bc.h);
        p.draw_char({ col, h - 1 }, bc.h);
    }

    // Left and right edges.
    for (int row = 1; row < h - 1; ++row) {
        p.draw_char({ 0,     row }, bc.v);
        p.draw_char({ w - 1, row }, bc.v);
    }

    // Title (optional): ┌─ Title ─┐
    if (!title_.empty() && w >= 6) {
        // Build " Title " padded with spaces on both sides.
        const std::string display = " " + title_ + " ";
        const int max_title_cols  = w - 4; // reserve 2 for corners + 1 each side
        // Truncate if needed.
        std::string rendered = display;
        if (static_cast<int>(display.size()) > max_title_cols) {
            rendered = display.substr(0, static_cast<std::size_t>(max_title_cols));
        }
        // Draw title starting at column 2, bold when focused (or a child is focused).
        p.set_color(saved_color);
        if (has_focused_descendant()) p.enable_style(pixel_style_flag::bold);
        p.draw_text({ 2, 0 }, rendered);
        if (has_focused_descendant()) p.disable_style(pixel_style_flag::bold);

        // Redraw the surrounding dashes in border color.
        if (border_color_ != color::default_color) {
            p.set_color(border_color_);
        }
        p.draw_char({ 1, 0 }, bc.h);
        const int end_col = 2 + static_cast<int>(rendered.size());
        if (end_col < w - 1) {
            p.draw_char({ end_col, 0 }, bc.h);
        }
    }
}

} // namespace nx::tui
