//
// Created by nexie on 09.03.2026.
//

#ifndef NX_TUI_SIZE_HPP
#define NX_TUI_SIZE_HPP

#include <type_traits>

namespace nx::tui {

    template<typename Type>
    struct SizeTraits
    {
        static_assert (std::is_integral<Type>::value || std::is_floating_point<Type>::value, "Template parameter 'Type' must be an integral or a floating point");
        using size_type = Type;
    };

    template <typename Type = int>
    struct Size {
        using traits_type = SizeTraits<Type>;

        traits_type::size_type height;
        traits_type::size_type width;
    };

    template<typename Type = int>
    [[nodiscard]] constexpr auto
    operator + (const Size<Type> & left, const Size<Type> & right) -> Size<Type>
    {
        return {left.height + right.height, left.width + right.width};
    }

    template<typename Type = int>
    [[nodiscard]] constexpr auto
    operator - (const Size<Type> & left, const Size<Type> & right) -> Size<Type>
    {
        return {left.height - right.height, left.width - right.width};
    }

    template<typename Type = int, typename Operand = Type>
    [[nodiscard]] constexpr auto
    operator * (const Size<Type> & left, const Operand & right) -> decltype(auto)
    {
        return Size(left.height * right, left.width * right);
    }

    template<typename Type = int, typename Operand = Type>
    [[nodiscard]] constexpr auto
    operator / (const Size<Type> & left, const Operand & right) -> decltype(auto)
    {
        return Size(left.height / right, left.width / right);
    }

    struct WindowSize {
        Size<int> chars;
        Size<int> pixels;
    };

    using Dimension = Size<size_t>;
}

#endif //NX_TUI_SIZE_HPP
