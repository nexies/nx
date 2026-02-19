//
// Created by nexie on 19.02.2026.
//

#ifndef NX_MACRO_ARGS_PARSE_2_HPP
#define NX_MACRO_ARGS_PARSE_2_HPP

#include <nx/macro.hpp>
#include <nx/macro/logic/if.hpp>
#include <nx/macro/logic/bool.hpp>
#include <nx/macro/repeating/iterate.hpp>
#include <nx/macro/args/append.hpp>
#include <nx/macro/args/count.hpp>

#include <nx/macro/repeating/while.hpp>
#include <nx/macro/numeric/compare.hpp>
#include <nx/macro/numeric/sum.hpp>
#include <nx/macro/tuple/tuple.hpp>

#include <nx/macro/args/token.hpp>
#include <nx/macro/args/argset.hpp>

# define _nx_args_tokenize_all_iterator(n, arg) _nx_args_tokenize(arg)

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

# define _nx_args_max_token_name_d(d, ...) \
    _nx_while_d(d)( \
        _nx_args_max_token_name_cond_d, \
        _nx_args_max_token_name_op_d, \
        _nx_args_max_token_name_res_d, \
        /*res*/ 0, /*tokens*/ __VA_ARGS__ \
    )

#define PARAM1 1
#define PARAM2 2
#define PARAM3 10

// #define tokens _nx_args_tokenize_all(PARAM1 HELLO, PARAM2 WORLD, PARAM3 333)

// _nx_args_max_token_name_op_d(0, 0, (PARAM1, HELLO), (PARAM2, WORLD))

// _nx_args_max_token_name_d(0, (PARAM1, HELLO), (PARAM2, WORLD), (PARAM3, 333))

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


_nx_args_to_argset_autosize(PARAM1 hello, PARAM2 world, PARAM3 one, PARAM3 another one)



// _nx_args_tokenize_all(PARAM1 hello, PARAM2 world, PARAM3 another one)


#endif //NX_MACRO_ARGS_PARSE_2_HPP