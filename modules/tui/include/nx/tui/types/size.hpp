//
// Created by nexie on 09.03.2026.
//

#ifndef NX_TUI_SIZE_HPP
#define NX_TUI_SIZE_HPP

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

    struct WindowSize {
        Size<int> chars;
        Size<int> pixels;
    };

    using Dimension = Size<size_t>;
}

#endif //NX_TUI_SIZE_HPP
