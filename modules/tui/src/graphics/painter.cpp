//
// Created by nexie on 20.03.2026.
//

#include <nx/tui/graphics/painter.hpp>

namespace nx::tui
{
    Painter::Painter(buffer_type& buffer)
        : buffer_(buffer)
        , rect_(buffer.rect())
    {

    }

    Painter::Painter(buffer_type& buffer, rect_type rect)
        : buffer_(buffer)
        , rect_(rect)
    {

    }

    void Painter::enableStyle(const PixelStyle& pixel_style)
    {
        pixel_style_ |= pixel_style;
    }

    void Painter::disableStyle(const PixelStyle& pixel_style)
    {
        pixel_style_ &= ~pixel_style;
    }

    void Painter::setStyle(const PixelStyle& pixel_style)
    {
        pixel_style_ = pixel_style;
    }

    void Painter::setColor(const Color& color)
    {
        color_ = color;
    }

    void Painter::setBackgroundColor(const Color& color)
    {
        background_color_ = color;
    }

    void Painter::drawText(const point_type & pos, const std::string& text) const
    {
        const auto abs_pos = _projectPoint(pos);

        for (auto i = 0; i < text.length(); ++i)
        {
            auto & pixel = buffer_.pixelAt(abs_pos.x + i, abs_pos.y);
            pixel.style = pixel_style_;
            pixel.background_color = background_color_;
            pixel.foreground_color = color_;
            pixel.character = text[i];
        }

    }

    constexpr Painter::point_type Painter::_projectPoint(const point_type & pos) const
    {
        return { pos.x + rect_.x(), pos.y + rect_.y() };
    }
}

