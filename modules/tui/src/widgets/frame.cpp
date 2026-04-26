#include <nx/tui/widgets/frame.hpp>
#include <nx/tui/graphics/painter.hpp>

#include <algorithm>
#include <array>
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

// ── frame_layout ──────────────────────────────────────────────────────────────
//
// Wraps an inner layout and offsets all children by (1,1) so they sit inside
// the border.  Children whose assigned size equals the full parent dimension
// are shrunk by 2 to fit inside the border.

class frame_layout : public layout {
    std::unique_ptr<layout> inner_;

public:
    void set_inner(std::unique_ptr<layout> l) { inner_ = std::move(l); }

    void apply(widget & parent) override
    {
        if (!inner_) return;

        const int pw = parent.size().width;
        const int ph = parent.size().height;
        if (pw < 3 || ph < 3) return;

        inner_->apply(parent);

        // Offset every child into the inner area.
        for (auto * child : parent.child_widgets()) {
            const auto g = child->geometry();
            const int  new_w = (g.width() == pw) ? pw - 2 : g.width();
            const int  new_h = (g.height() == ph) ? ph - 2 : g.height();
            child->set_geometry({ g.x() + 1, g.y() + 1, new_w, new_h });
        }
    }
};

// ── frame ─────────────────────────────────────────────────────────────────────

frame::frame(nx::core::object * parent)
    : widget(parent)
{
    auto fl = std::make_unique<frame_layout>();
    widget::set_layout(std::move(fl));
}

void frame::set_content_layout(std::unique_ptr<layout> l)
{
    // The widget already owns a frame_layout; update its inner layout.
    auto * fl = static_cast<frame_layout *>(get_layout());
    fl->set_inner(std::move(l));
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

    // Fill inner area first (inherits painter's current fg/bg/style).
    // Temporarily reset to saved color so the fill uses content style.
    p.set_color(saved_color);
    for (int row = 1; row < h - 1; ++row) {
        for (int col = 1; col < w - 1; ++col) {
            p.draw_char({ col, row }, " ");
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
        // Draw title starting at column 2.
        p.set_color(saved_color);
        p.draw_text({ 2, 0 }, rendered);
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
