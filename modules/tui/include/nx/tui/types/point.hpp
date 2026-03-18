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
        using traits_type = SizeTraits<Type>;
        using size_type = typename traits_type::size_type;

        size_type x;
        size_type y;
    };


}

#endif //NX_TUI_POINT_HPP