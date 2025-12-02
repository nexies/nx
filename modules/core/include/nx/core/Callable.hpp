//
// Created by nexie on 02.12.2025.
//

#ifndef CALLABLE_HPP
#define CALLABLE_HPP

namespace nx {

    namespace detail {

        template<typename... Ts> struct List { static constexpr auto size = sizeof...(Ts); };
        template<typename Head,  typename... Tail> struct List<Head, Tail...> {
            using Value = Head;
            using Rest = List<Tail...>;
            static constexpr auto size = List<Tail...>::size + 1;
        };

        /// FunctionDescriptor has to detect:
        ///     1) return value (all)
        ///     2) parameters types (all)
        ///     3) noexcept (all)
        ///     4) const (member functions)
        ///     5) volatile (member functions)
        ///     6) const volatile (member functions)
        ///     7) variadic ... (all) ??
        ///     8) extern "C" (all) ??
        ///     9) extern "C++" (all) ??
        ///     10) ref & (member functions) ??
        ///     11) ref && (member functions) ??

        enum class FunctionClassification {
            Function,
            MemberFunction,
        };


    }

}

#endif //CALLABLE_HPP
