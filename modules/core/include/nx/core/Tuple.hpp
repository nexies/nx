//
// Created by nexie on 03.12.2025.
//

#ifndef TUPLE_HPP
#define TUPLE_HPP
#include <cstddef>
#include <utility>

namespace nx
{
    template<typename... Ts>
    struct Tuple;

    template<>
    struct Tuple<> {
        static constexpr std::size_t size = 0;

        Tuple() = default;

        template<typename F>
        decltype(auto) apply_to(F&& f) &&
        {
            return std::invoke(std::forward<F>(f));
        }

        template<typename F>
        decltype(auto) apply_to(F&& f) &
        {
            return std::invoke(std::forward<F>(f));
        }
    };

    template<typename Head, typename... Tail>
    struct Tuple<Head, Tail...> : Tuple<Tail...>
    {
    protected:
        using Base = Tuple<Tail...>;
        using Car  = Head;
        Car value;

    public:
        static constexpr std::size_t Size = sizeof...(Tail) + 1;
        Tuple() = default;

        template<typename H, typename... Ts>
        explicit Tuple(H&& h, Ts&&... ts)
            : Base(std::forward<Ts>(ts)...)
            , value(std::forward<H>(h))
        {}

        constexpr size_t size () { return Size; }

        template<std::size_t I>
        decltype(auto) get() & {
            if constexpr (I == 0) {
                return (value);
            } else {
                Base & base = *this;
                return base.template get<I - 1>();
            }
        }

        template<std::size_t I>
        decltype(auto) get() const & {
            if constexpr (I == 0) {
                return (value);
            } else {
                const Base & base = *this;
                return base.template get<I - 1>();
            }
        }

        template<std::size_t I>
        decltype(auto) get() && {
            if constexpr (I == 0) {
                return std::move(value);
            } else {
                Base && base = std::move(*this);
                return base.template get<I - 1>();
            }
        }

        template<typename F>
        decltype(auto) apply_to(F&& f) & {
            constexpr std::size_t N = Size;
            return apply_impl(
                std::forward<F>(f),
                std::move(*this),
                std::make_index_sequence<Size>{}
            );
        }

        template<typename F>
        decltype(auto) apply_to(F&& f) && {
            constexpr std::size_t N = Size;
            return apply_impl(
                std::forward<F>(f),
                std::forward<Tuple>(*this),
                std::make_index_sequence<Size>{}
            );
        }

    private:
        template<typename F, typename TupleT, std::size_t... I>
        static decltype(auto) apply_impl(F&& f, TupleT&& t, std::index_sequence<I...>) {
            return std::invoke(
                std::forward<F>(f),
                std::forward<TupleT>(t).template get<I>()...
            );
        }

    };

    template<typename Head, typename... Tail>
    Tuple(Head, Tail...) -> Tuple<Head, Tail...>;


}

#endif //TUPLE_HPP
