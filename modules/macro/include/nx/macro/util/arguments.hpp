//
// Created by nexie on 08.12.2025.
//

#ifndef ARGNAME_HPP
#define ARGNAME_HPP

#include <nx/macro.hpp>

#include <nx/macro/detail/arg_tk_limit.hpp>
#include <nx/macro/logic/if.hpp>
#include <nx/macro/logic/bool.hpp>
#include <nx/macro/repeating/iterate.hpp>
#include <nx/macro/util/append_args.hpp>

#include <nx/macro/numeric/compare.hpp>
#include <nx/macro/tuple/tuple.hpp>

///
/// #define PARAM1 1
/// #define PARAM2 2
/// #define NAME 3
/// #define TYPE 4
///
///
/// #define _nx_arguments_contains(name, ...)   -> 1 or 0,
/// #define _nx_arguments_get(name, ...)        -> list of passed values
/// #define _nx_arguments_get_tuple(name, ...)  -> tuple, containing the passed values
/// #define _nx_arguments_count(name)           -> element count
///
/// #define MY_MACRO_WITH_NAMED_PARAMS(...) \
///     _nx_logic_if(_nx_arguments_contains(PARAM1, __VA_ARGS__))
///
/// MY_MACRO_WITH_NAMED_PARAMS(PARAM1 val1, PARAM2 val2, NAME val3, TYPE val4)




#define _nx_arguments_arg_to_tuple(named_argument) \
    _nx_concat_2(_nx_arg_tk_, named_argument) )

#define _nx_arguments_arg_apply_macro(macro, named_argument) \
    macro _nx_arguments_arg_to_tuple(named_argument)

#define _nx_arguments_arg_has_name(name, tuple) \
    _nx_numeric_eq(name, _nx_tuple_get(0, tuple))

// #define _nx_arguments_contains_iterator(n, name, ...) \
//     _nx_logic_if( \
//             _nx_arguments_arg_has_name( \
//                 name, \
//                 _nx_arguments_arg_to_tuple( \
//                     _nx_choose(n,, __VA_ARGS__)\
//                 )\
//             )\
//         ) \
//     (\
//         1, \
//         0 \
//     )
//
// #define _nx_arguments_contains(name, ...) \
//     _nx_make_sequence(\
//         _nx_args_count(__VA_ARGS__), \
//         _nx_arguments_contains_iterator, \
//         name, __VA_ARGS__ \
//     )


/// INPUT(1 123, 2 123, 3 123)


#endif //ARGNAME_HPP
