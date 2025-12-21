//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_SEQUENCE_HPP
#define NX_MACRO_SEQUENCE_HPP

#include <nx/macro.hpp>

#include <nx/macro/detail/sequence_limit.hpp>

#define _nx_make_decorated_sequence(count, decorator, macro, ...) \
_nx_concat_2(_nx_sequence_, count)(2, decorator, macro, __VA_ARGS__)

#define _nx_sequence_comma_separator_0(data) data,
#define _nx_sequence_comma_separator_1(data) data,
#define _nx_sequence_comma_separator_2(data) data
#define _nx_sequence_comma_separator(c, data) _nx_concat_2(_nx_sequence_comma_separator_, c)(data)

#define _nx_sequence_no_decorator(c, data) data

#define _nx_make_sequence(count, macro, ...) \
_nx_make_decorated_sequence(count, _nx_sequence_comma_separator, macro, __VA_ARGS__)

#define _nx_make_sequence_no_decorator(count, macro, ...) \
_nx_make_decorated_sequence(count, _nx_sequence_no_decorator, macro, __VA_ARGS__)


/// @param count
/// @param decorator
/// @param macro
#define NX_DECORATED_SEQUENCE(count, decorator, macro, ...) \
    _nx_make_sequence_with_decorator(count, decorator, macro, __VA_ARGS__)

///
/// @param count
#define NX_MAKE_SEQ(count) \
    _nx_make_sequence(count, _nx_expand)

///
/// @param count
#define NX_PLACEHOLDERS(count) \
    _nx_make_sequence(count, _nx_rconcat, _)





#endif //NX_MACRO_ITERATE_HPP
