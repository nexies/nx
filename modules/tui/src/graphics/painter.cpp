#include <nx/tui/graphics/painter.hpp>

namespace nx::tui {

painter::painter(buffer_type & buffer)
    : buffer_(buffer)
    , rect_(buffer.rect())
{}

painter::painter(buffer_type & buffer, rect_type clip_rect)
    : buffer_(buffer)
    , rect_(clip_rect)
{}

void painter::enable_style(pixel_style style)  { pixel_style_ |= style; }
void painter::disable_style(pixel_style style) { pixel_style_ &= ~style; }
void painter::set_style(pixel_style style)     { pixel_style_ = style; }
void painter::set_color(const color & c)       { color_ = c; }
void painter::set_background_color(const color & c) { background_color_ = c; }

void painter::draw_text(const point_type & pos, const std::string & text) const
{
    const int base_x = rect_.x() + pos.x;
    const int by     = rect_.y() + pos.y;

    // Clip row.
    if (by < rect_.y() || by >= rect_.y() + rect_.height()) return;

    // Iterate UTF-8 characters; each occupies exactly one terminal cell.
    int         col = 0;
    std::size_t i   = 0;
    while (i < text.size()) {
        // Determine byte length of this UTF-8 character.
        const auto c = static_cast<unsigned char>(text[i]);
        std::size_t char_len;
        if      (c < 0x80) char_len = 1;
        else if (c < 0xE0) char_len = 2;
        else if (c < 0xF0) char_len = 3;
        else               char_len = 4;
        // Guard against malformed / truncated input.
        if (i + char_len > text.size()) char_len = text.size() - i;

        const int bx = base_x + col;
        if (bx >= rect_.x() + rect_.width()) break; // past right edge

        if (bx >= rect_.x()) { // within horizontal clip
            auto & px           = buffer_.pixel_at(bx, by);
            px.style            = pixel_style_;
            px.background_color = background_color_;
            px.foreground_color = color_;
            px.character        = std::string(text.data() + i, char_len);
        }

        i += char_len;
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
            auto & px           = buffer_.pixel_at(rect_.x() + col, rect_.y() + row);
            px.style            = pixel_style_;
            px.background_color = background_color_;
            px.foreground_color = color_;
            px.character        = ch;
        }
    }
}

void painter::apply_style(const style_option & s) noexcept
{
    if (s.foreground)  set_color(*s.foreground);
    if (s.background)  set_background_color(*s.background);
    if (s.decorations) set_style(*s.decorations);
}

painter::point_type painter::_project_point(const point_type & pos) const
{
    return { pos.x + rect_.x(), pos.y + rect_.y() };
}

} // namespace nx::tui
