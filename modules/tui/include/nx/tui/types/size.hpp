#pragma once

#include <type_traits>

namespace nx::tui {

    template<typename T>
    struct size_traits
    {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                      "size_traits<T>: T must be integral or floating-point");
        using value_type = T;
    };

    template<typename T = int>
    struct size {
        using traits_type = size_traits<T>;
        using value_type  = typename traits_type::value_type;

        value_type height {};
        value_type width  {};

        [[nodiscard]] constexpr bool
        operator==(const size & o) const noexcept
        { return height == o.height && width == o.width; }

        [[nodiscard]] constexpr bool
        operator!=(const size & o) const noexcept
        { return !(*this == o); }
    };

    template<typename T = int>
    [[nodiscard]] constexpr size<T>
    operator+(const size<T> & a, const size<T> & b) noexcept
    {
        return { a.height + b.height, a.width + b.width };
    }

    template<typename T = int>
    [[nodiscard]] constexpr size<T>
    operator-(const size<T> & a, const size<T> & b) noexcept
    {
        return { a.height - b.height, a.width - b.width };
    }

    template<typename T = int, typename S = T>
    [[nodiscard]] constexpr size<T>
    operator*(const size<T> & a, S scalar) noexcept
    {
        return { static_cast<T>(a.height * scalar), static_cast<T>(a.width * scalar) };
    }

    template<typename T = int, typename S = T>
    [[nodiscard]] constexpr size<T>
    operator/(const size<T> & a, S scalar) noexcept
    {
        return { static_cast<T>(a.height / scalar), static_cast<T>(a.width / scalar) };
    }

    // Physical terminal window size: character grid + pixel dimensions.
    struct window_size {
        size<int> chars;
        size<int> pixels;
    };

    using dimension = size<std::size_t>;

} // namespace nx::tui
