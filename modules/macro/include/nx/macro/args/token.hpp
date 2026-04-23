//
// Created by nexie on 19.02.2026.
//

#ifndef NX_MACRO_ARGS_TOKEN_HPP
#define NX_MACRO_ARGS_TOKEN_HPP

#include <nx/macro.hpp>
#include <nx/macro/detail/arg_tk_limit.hpp>

#include <nx/macro/tuple/tuple.hpp>

#include <nx/macro/numeric/inc_dec.hpp>



/// Tokenize @p arg
///
/// @p arg is an expression: NAME VALUE
///
/// @p token is a tuple: (NAME, VALUE)
///
/// @param arg argument to tokenize. Argument's NAME should define a valid integer in range from 0 to @p NX_MAX_NUMBER
# define _nx_args_tokenize(arg) \
    _nx_concat_2(_nx_arg_tk_, arg) )


/// Get token's name
///
/// (NAME, VALUE) -> NAME
# define _nx_args_token_name(token) \
    _nx_tuple_get(0, token)

/// Get token's value
///
/// (NAME, VALUE) -> VALUE
# define _nx_args_token_value(token) \
    _nx_logic_if(_nx_bool(_nx_dec(_nx_tuple_size(token)))) ( \
        _nx_tuple_get(1, token), \
        _nx_empty() \
    )


// // # define _nx_args_token_has_value(token) \
//     NX_BOOL(_nx_dec(_nx_tuple_size(token)))

/// Check that token @p token has name @p name
/// @param d
/// @param name
/// @param token
# define _nx_args_token_name_is_d(d, name, token) \
    _nx_numeric_eq_d(d, name, _nx_args_token_name(token))

/// Check that token @p token has name @p name
/// @param name
/// @param token
# define _nx_args_token_name_is(name, token) \
    _nx_args_token_name_is_d(0, name, token)

/// NAME VALUE -> NAME
# define _nx_args_arg_name(arg) \
    _nx_args_token_name(_nx_args_tokenize(arg))

/// NAME VALUE -> VALUE
# define _nx_args_arg_value(arg) \
    _nx_args_token_value(_nx_args_tokenize(arg))

/// Check that named argument @p arg has the name @p name
/// @param d
/// @param name
/// @param arg
# define _nx_args_arg_name_is_d(d, name, arg) \
    _nx_args_token_name_is_d(d, name, _nx_args_tokenize(arg))

/// Check that named argument @p arg has the name @p name
/// @param name
/// @param arg
# define _nx_args_arg_name_is(name, arg) \
    _nx_args_token_name_is(name, _nx_args_tokenize(arg))

# define _nx_args_tokenize_all_iterator(n, arg) _nx_args_tokenize(arg)

/// Tokenize all the arguments in the sequence
# define _nx_args_tokenize_all(...) \
    NX_DECORATED_ITERATE(_nx_args_tokenize_all_iterator, NX_COMMA_DECORATOR, __VA_ARGS__)

# define _nx_args_max_token_name_cond_d(d, res, ...) \
    NX_HAS_ARGS(__VA_ARGS__)

# define _nx_args_max_token_name_op_d(d, res, cur_token, ...) \
    NX_IF(NX_GREATER_D(d, _nx_args_token_name(cur_token), res)) ( \
        _nx_expand(_nx_args_token_name(cur_token) NX_APPEND_VA_ARGS(__VA_ARGS__)), \
        _nx_expand(res NX_APPEND_VA_ARGS(__VA_ARGS__)) \
    )

# define _nx_args_max_token_name_res_d(d, res, ...) \
    res

/// Find name with the max value among tokens in the sequence
# define _nx_args_max_token_name_d(d, ...) \
    _nx_while_d(d)( \
        _nx_args_max_token_name_cond_d, \
        _nx_args_max_token_name_op_d, \
        _nx_args_max_token_name_res_d, \
        /*res*/ 0, /*tokens*/ __VA_ARGS__ \
    )

#endif //NX_MACRO_ARGS_TOKEN_HPP