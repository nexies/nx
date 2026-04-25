#pragma once

#include <nx/tui/types/size.hpp>

namespace nx::tui {

    template<typename T = int>
    struct point
    {
        using value_type = typename size_traits<T>::value_type;

        value_type x {};
        value_type y {};

        constexpr point() noexcept = default;

        constexpr point(value_type p_x, value_type p_y) noexcept
            : x { p_x }, y { p_y }
        {}

        // Construct from size (width -> x, height -> y).
        explicit constexpr point(size<T> s) noexcept
            : x { s.width }, y { s.height }
        {}
    };

} // namespace nx::tui
