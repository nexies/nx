#include <nx/tui/graphics/painter.hpp>
#include <nx/common/types/utf8.hpp>

namespace nx::tui {

// ── construction ──────────────────────────────────────────────────────────────

painter::painter(buffer_type & buffer)
    : buffer_(buffer)
    , rect_(buffer.rect())
{}

painter::painter(buffer_type & buffer, rect_type clip_rect)
    : buffer_(buffer)
    , rect_(clip_rect)
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
    auto s = _at(lc, lr);
    auto & px           = buffer_.pixel_at(bx, by);
    px.character        = ch;
    px.foreground_color = s.foreground.value_or(color::default_color);
    px.background_color = s.background.value_or(color::default_color);
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

    for (auto it = nx::utf8::view(text).begin(),
              end = nx::utf8::view(text).end(); it != end; ++it) {
        const int bx = base_x + col;
        if (bx >= rect_.x() + rect_.width()) break;

        auto g = *it;
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
    for (int row = 0; row < rect_.height(); ++row) {
        for (int col = 0; col < rect_.width(); ++col) {
            _write(rect_.x() + col, rect_.y() + row, col, row, ch);
        }
    }
}

painter::point_type painter::_project_point(const point_type & pos) const
{
    return { pos.x + rect_.x(), pos.y + rect_.y() };
}

} // namespace nx::tui
