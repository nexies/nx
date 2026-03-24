//
// Created by nexie on 17.03.2026.
//

#include <nx/tui/graphics/display_buffer.hpp>
#include <nx/tui/graphics/painter.hpp>

using namespace nx::tui;

thread_local DisplayBuffer::pixel_type g_dummy_pixel;

DisplayBuffer::DisplayBuffer(units hrow, units wcol)
    : rect_({hrow, wcol})
{
    pixels_.resize(hrow * wcol);
}

DisplayBuffer::DisplayBuffer(Size<units> size) :
    DisplayBuffer(size.height, size.width)
{

}

DisplayBuffer::~DisplayBuffer()
{
}

Painter DisplayBuffer::getPainter()
{
    return Painter(*this);
}

void DisplayBuffer::resize(units hrow, units wcol)
{
    rect_ = Rect<units>({hrow, wcol});
    pixels_.resize(hrow * wcol);
}

DisplayBuffer::pixel_reference DisplayBuffer::pixelAt(units x, units y)
{
    if (!rect_.contains(x, y))
        return g_dummy_pixel;
    return pixels_[x + y * rect_.width()];
}

DisplayBuffer::const_pixel_reference DisplayBuffer::pixelAt(units x, units y) const
{
    if (!rect_.contains(x, y))
        return g_dummy_pixel;
    return pixels_[x + y * rect_.width()];
}

DisplayBuffer::character_type& DisplayBuffer::at(units x, units y)
{
    return pixelAt(x, y).character;
}

const DisplayBuffer::character_type& DisplayBuffer::at(units x, units y) const
{
    return pixelAt(x, y).character;
}

DisplayBuffer::units DisplayBuffer::hrow() const
{
    return rect_.height();
}

DisplayBuffer::units DisplayBuffer::wcol() const
{
    return rect_.width();
}

void DisplayBuffer::clear()
{
    for (auto & pixel : pixels_)
        pixel = Pixel{};
}

DisplayBuffer::size_type DisplayBuffer::size() const
{
    return rect_.size();
}

DisplayBuffer::rect_type DisplayBuffer::rect() const
{
    return rect_;
}

const std::vector<DisplayBuffer::pixel_type>& DisplayBuffer::data() const
{
    return pixels_;
}
