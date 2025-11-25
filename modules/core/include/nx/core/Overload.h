//
// Created by nexie on 11.11.2025.
//

#ifndef OVERLOAD_H
#define OVERLOAD_H

#include "Functor.hpp"

namespace nx
{
    namespace detail
    {
        template<typename... Args>
        struct non_const_overload_t
        {
            template<typename Ret, typename Class>
            constexpr auto operator () (Ret(Class::*f)(Args...)) const noexcept -> decltype(f)
            { return f; }
        };

        template<typename... Args>
        struct const_overload_t
        {
            template<typename Ret, typename Class>
            constexpr auto operator () (Ret(Class::*f)(Args...) const) const noexcept -> decltype(f)
            { return f; }
        };

        template<typename... Args>
        struct overload_t : detail::non_const_overload_t<Args...>, detail::const_overload_t<Args...>
        {
            using detail::non_const_overload_t<Args...>::operator();
            using detail::const_overload_t<Args...>::operator();

            template<typename Ret>
            constexpr auto operator () (Ret(*f)(Args...)) const noexcept -> decltype(f)
            { return f; }
        };

    }

    template<typename... Args> constexpr inline detail::overload_t<Args...> overload = {};
    template<typename... Args> constexpr inline detail::const_overload_t<Args...> const_overload = {};
    template<typename... Args> constexpr inline detail::non_const_overload_t<Args...> non_const_overload = {};

}

#endif //OVERLOAD_H