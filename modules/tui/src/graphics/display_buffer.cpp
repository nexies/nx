#include <nx/tui/graphics/display_buffer.hpp>
#include <nx/tui/graphics/painter.hpp>

using namespace nx::tui;

thread_local display_buffer::pixel_type g_dummy_pixel;

display_buffer::display_buffer(units rows, units cols)
    : rect_({ rows, cols })
{
    pixels_.resize(rows * cols);
}

display_buffer::display_buffer(size_type s)
    : display_buffer(s.height, s.width)
{}

display_buffer::~display_buffer() = default;

painter display_buffer::get_painter()
{
    return painter(*this);
}

void display_buffer::resize(units rows, units cols)
{
    rect_ = rect_type({ rows, cols });
    pixels_.resize(rows * cols);
}

display_buffer::pixel_reference display_buffer::pixel_at(units x, units y)
{
    if (!rect_.contains(x, y))
        return g_dummy_pixel;
    return pixels_[x + y * rect_.width()];
}

display_buffer::const_pixel_reference display_buffer::pixel_at(units x, units y) const
{
    if (!rect_.contains(x, y))
        return g_dummy_pixel;
    return pixels_[x + y * rect_.width()];
}

display_buffer::character_type & display_buffer::at(units x, units y)
{
    return pixel_at(x, y).character;
}

const display_buffer::character_type & display_buffer::at(units x, units y) const
{
    return pixel_at(x, y).character;
}

void display_buffer::clear()
{
    for (auto & px : pixels_)
        px = pixel{};
}
