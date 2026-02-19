//
// Created by nexie on 19.02.2026.
//

#ifndef NX_MACRO_ARGS_TOKEN_HPP
#define NX_MACRO_ARGS_TOKEN_HPP

#include <nx/macro.hpp>
#include <nx/macro/detail/arg_tk_limit.hpp>

#include <nx/macro/tuple/tuple.hpp>




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
    _nx_tuple_get(1, token)

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


#endif //NX_MACRO_ARGS_TOKEN_HPP