//
// Created by nexie on 17.03.2026.
//

#include <nx/tui/graphics/canvas.hpp>

using namespace nx::tui;

thread_local Canvas::pixel_type g_dummy_pixel;

Canvas::Canvas(size_type hrow, size_type wcol)
    : rect_({hrow, wcol})
{
    pixels_.resize(hrow * wcol);
}

Canvas::~Canvas()
{
}

void Canvas::resize(size_type hrow, size_type wcol)
{
    rect_ = Rect<size_type>({hrow, wcol});
    pixels_.resize(hrow * wcol);
}

Canvas::pixel_reference Canvas::pixelAt(size_type x, size_type y)
{
    if (!rect_.contains(x, y))
        return g_dummy_pixel;
    return pixels_[x + y * rect_.width()];
}

Canvas::const_pixel_reference Canvas::pixelAt(size_type x, size_type y) const
{
    if (!rect_.contains(x, y))
        return g_dummy_pixel;
    return pixels_[x + y * rect_.width()];
}

Canvas::character_type& Canvas::at(size_type x, size_type y)
{
    return pixelAt(x, y).character;
}

const Canvas::character_type& Canvas::at(size_type x, size_type y) const
{
    return pixelAt(x, y).character;
}

Canvas::size_type Canvas::hrow() const
{
    return rect_.height();
}

Canvas::size_type Canvas::wcol() const
{
    return rect_.width();
}

void Canvas::clear()
{
    for (auto & pixel : pixels_)
        pixel = Pixel{};
}
