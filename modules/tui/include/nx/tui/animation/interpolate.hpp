#pragma once

#include <nx/tui/types/color.hpp>
#include <nx/tui/types/point.hpp>
#include <nx/tui/types/size.hpp>
#include <nx/tui/types/rect.hpp>

namespace nx::tui {

// Primary template — T must be specialised below.
template <typename T>
T interpolate(float t, const T & from, const T & to);

template <>
inline float interpolate<float>(float t, const float & from, const float & to)
{
    return from + (to - from) * t;
}

template <>
inline double interpolate<double>(float t, const double & from, const double & to)
{
    return from + (to - from) * static_cast<double>(t);
}

template <>
inline int interpolate<int>(float t, const int & from, const int & to)
{
    return static_cast<int>(static_cast<float>(from)
                          + static_cast<float>(to - from) * t + 0.5f);
}

template <>
inline color interpolate<color>(float t, const color & from, const color & to)
{
    return color::interpolate(t, from, to);
}

template <>
inline point<int> interpolate<point<int>>(float t,
                                          const point<int> & from,
                                          const point<int> & to)
{
    return { interpolate<int>(t, from.x, to.x),
             interpolate<int>(t, from.y, to.y) };
}

template <>
inline size<int> interpolate<size<int>>(float t,
                                        const size<int> & from,
                                        const size<int> & to)
{
    return { interpolate<int>(t, from.height, to.height),
             interpolate<int>(t, from.width,  to.width)  };
}

template <>
inline rect<int> interpolate<rect<int>>(float t,
                                        const rect<int> & from,
                                        const rect<int> & to)
{
    return { interpolate<int>(t, from.x(),      to.x()),
             interpolate<int>(t, from.y(),      to.y()),
             interpolate<int>(t, from.width(),  to.width()),
             interpolate<int>(t, from.height(), to.height()) };
}

} // namespace nx::tui
