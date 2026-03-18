//
// Created by nexie on 18.03.2026.
//

#ifndef NX_ENUM_HPP
#define NX_ENUM_HPP

#ifdef __nx_cxx_20_enum

#include "fmt/format.h"
#include "nx/experimental/reflect"
#include <type_traits>


namespace nx::detail
{
    template <class E>
    constexpr std::string_view enum_type_name() {
        return reflect::type_name<E>().empty() ? "UNKNOWN_ENUM" : reflect::type_name<E>();
    }

    template <class E>
    constexpr std::string_view enum_value_name(E value) {
        return reflect::enum_name(value).empty() ? "INVALID_VALUE" : reflect::enum_name(value);
    }

    enum class enum_fmt_mode {
        name,       // {}
        qualified,  // {:q}
        integer     // {:d}
    };
}

FMT_BEGIN_NAMESPACE
template<typename EnumType, typename Char>
struct formatter<EnumType, Char, std::enable_if_t<std::is_enum<EnumType>::value>>
{
    nx::detail::enum_fmt_mode mode = nx::detail::enum_fmt_mode::name;

    constexpr auto parse(basic_format_parse_context<Char>& ctx)
        -> typename basic_format_parse_context<Char>::iterator
    {
        auto it = ctx.begin();
        auto end = ctx.end();

        if (it == end || *it == static_cast<Char>('}')) {
            return it;
        }

        switch (*it) {
        case static_cast<Char>('q'):
            mode = nx::detail::enum_fmt_mode::qualified;
            ++it;
            break;
        case static_cast<Char>('d'):
            mode = nx::detail::enum_fmt_mode::integer;
            ++it;
            break;
        default:
            throw format_error("invalid format specifier for enum");
        }

        if (it != end && *it != static_cast<Char>('}')) {
            throw format_error("invalid format specifier for enum");
        }

        return it;
    }

    template <typename FormatContext>
    auto format(EnumType v, FormatContext& ctx) const
        -> FormatContext::iterator
    {
        switch (mode)
        {
            case nx::detail::enum_fmt_mode::name:
                return fmt::format_to(ctx.out(), "{}", reflect::enum_name<EnumType>(v));
        case nx::detail::enum_fmt_mode::qualified:
                return fmt::format_to(
                    ctx.out(), "{}::{}",
                    nx::detail::enum_type_name<EnumType>(), nx::detail::enum_value_name<EnumType>(v));
            case nx::detail::enum_fmt_mode::integer:
                return fmt::format_to(ctx.out(), "{}", std::underlying_type_t<EnumType>(v));
        }
    }
};

FMT_END_NAMESPACE
#endif //__nx_cxx_20_enum
#endif //NX_ENUM_HPP
