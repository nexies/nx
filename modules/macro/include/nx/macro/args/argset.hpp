//
// Created by nexie on 19.02.2026.
//

#ifndef NX_ARGSET_HPP
#define NX_ARGSET_HPP

#include <nx/macro.hpp>
#include <nx/macro/tuple/tuple.hpp>

#include <nx/macro/repeat.hpp>
#include <nx/macro/numeric/compare.hpp>
#include <nx/macro/util/choose.hpp>
#include <nx/macro/util/put_at.hpp>
#include <nx/macro/util/overload.hpp>

#include <nx/macro/args/token.hpp>

/// Argset represents a two-dimensional table of arguments produced while
/// parsing macro input with named parameters.
///
/// The first index corresponds to the argument @p NAME, which must be defined
/// as a positive integer in the range [0, @p NX_MAX_NUMBER].
///
/// The second index represents the occurrence index of that argument in the
/// macro invocation. This allows handling multiple parameters with the same
/// name.
#define _nx_args_argset(size) \
    _nx_args_make_argset(size)

# define _nx_args_make_argset_iterator(n) \
    NX_TUPLE()

# define _nx_args_make_argset(count) \
    NX_TUPLE( NX_SEQUENCE(count, _nx_args_make_argset_iterator) )

/// Check that argset contains arguments with name @name
# define _nx_args_argset_contains(argset, name) \
    NX_TUPLE_NOT_EMPTY(NX_TUPLE_GET(argset, name))

# define _nx_args_argset_count(argset, name) \
    NX_TUPLE_SIZE(NX_TUPLE_GET(argset, name))

# define _nx_args_argset_contains_single_d(d, argset, name) \
    NX_EQUAL_D(d, 1, _nx_args_argset_count(argset, name))

# define _nx_args_argset_contains_single(argset, name) \
    _nx_args_argset_contains_single_d(0, argset, name)

# define _nx_args_argset_contains_many_d(d, argset, name) \
    NX_LESS_D(d, 1, _nx_args_argset_count(argset, name))

# define _nx_args_argset_contains_many(argset, name) \
    _nx_args_argset_contains_many_d(0, argset, name)

# define _nx_args_argset_get_as_tuple(argset, name) \
    NX_TUPLE_GET(argset, name)

# define _nx_args_argset_set_as_tuple(argset, name, tuple) \
    NX_TUPLE_SET(argset, name, tuple)

# define _nx_args_argset_append(argset, name, value) \
    _nx_args_argset_set_as_tuple(argset, name, _nx_tuple_append(_nx_args_argset_get_as_tuple(argset, name), value))

# define _nx_args_argset_get_by_index(argset, name, index) \
    NX_TUPLE_GET(NX_TUPLE_GET(argset, name), index)

# define _nx_args_argset_get_first(argset, name) \
    _nx_args_argset_get_by_index(argset, name, 0)

/// Get a parameter value from argset by its' name and positional index ( if parameter has more than one value )
# define _nx_args_argset_get_overloaded_3(argset, name, idx) \
    _nx_args_argset_get_by_index(argset, name, idx)

/// Get a parameter value by its' name.
/// If parameter has multiple values - returns as tuple
/// If parameter has single value - returns as value
# define _nx_args_argset_get_overloaded_2(argset, name) \
    _nx_logic_if(_nx_args_argset_contains_many(argset, name)) ( \
        _nx_args_argset_get_as_tuple(argset, name), \
        _nx_args_argset_get_by_index(argset, name, 0) \
    )

# define _nx_args_argset_get(...) \
    NX_OVERLOAD(_nx_args_argset_get_overloaded, __VA_ARGS__)



# define _nx_args_convert_to_argset_cond_d(d, argset, ...) \
    NX_HAS_ARGS(__VA_ARGS__)

# define _nx_args_convert_to_argset_op_d(d, argset, token, ...) \
    _nx_args_argset_append(argset, _nx_args_token_name(token), _nx_args_token_value(token)) NX_APPEND_VA_ARGS(__VA_ARGS__)

# define _nx_args_convert_to_argset_res_d(d, argset, ...) \
    argset

# define _nx_args_convert_to_argset_d(d, argset, ...) \
    _nx_while_d(d)( \
        _nx_args_convert_to_argset_cond_d, \
        _nx_args_convert_to_argset_op_d, \
        _nx_args_convert_to_argset_res_d, \
        argset, __VA_ARGS__ \
    )

# define _nx_args_tokens_to_argset(max_token_name, ...) \
    _nx_args_convert_to_argset_d(0, _nx_args_argset(NX_INC(max_token_name)), __VA_ARGS__)

# define _nx_args_tokens_to_argset_autosize(...) \
    _nx_args_tokens_to_argset(_nx_args_max_token_name_d(0, __VA_ARGS__), __VA_ARGS__)

# define _nx_args_to_argset_autosize(...) \
    _nx_args_tokens_to_argset_autosize(_nx_args_tokenize_all(__VA_ARGS__))



#endif //NX_ARGSET_HPP