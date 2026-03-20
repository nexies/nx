//
// Created by nexie on 17.03.2026.
//

#ifndef NX_TUI_POINT_HPP
#define NX_TUI_POINT_HPP

#include "size.hpp"

namespace nx::tui
{

    template<typename Type = int>
    struct Point
    {
        using units = SizeTraits<Type>::size_type;

        Point(units p_x, units p_y) noexcept
            : x {p_x}
            , y {p_y}
        {}

        Point()
            : x { 0 }
            , y { 0 }
        {}

        Point(Size<Type> p_size) noexcept
            : x { p_size.width }
            , y { p_size.height }
        {}

        units x;
        units y;
    };


}

#endif //NX_TUI_POINT_HPP