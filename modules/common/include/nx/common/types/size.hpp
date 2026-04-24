//
// Created by nexie on 17.04.2026.
//

#ifndef NX_COMMON_SIZE_HPP
#define NX_COMMON_SIZE_HPP

#include <type_traits>

namespace nx {

    namespace detail {
        template<typename Units, int Rank>
        struct size_traits {
            static_assert(Rank > 0, "Order template parameter must be greater than 0");
            static_assert(std::is_integral<Units>::value ||
                std::is_floating_point<Units>::value, "Unit must be an integral or a floating point");

            using units = Units;
            static constexpr auto rank = Rank;
        };
    }

    template<typename Units, int Rank>
    struct size_type {

    };

    template<typename Units>
    using nx::size_type<Units, 1> = Units;
}

#endif //NX_COMMON_SIZE_HPP
