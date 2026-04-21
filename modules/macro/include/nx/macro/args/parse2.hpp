//
// Created by nexie on 19.02.2026.
//

#ifndef NX_MACRO_ARGS_PARSE_2_HPP
#define NX_MACRO_ARGS_PARSE_2_HPP

#include <nx/macro/util/platform.hpp>
#include <nx/macro/args/argset.hpp>
#include <nx/common/helpers.hpp>
#include <nx/macro/args/rules.hpp>


# define _nx_args_parse2(parser, argset)                            \
    _nx_apply(parser, argset)

# define _nx_args_parse2_error(desc)                                \
    NX_STATIC_ASSERT(false, NX_TO_STRING(desc))


# define _nx_args_parse2_continue1(checkres, parser, argset)        \
    _nx_logic_if(_nx_tuple_get(0, checkres)) (                      \
        _nx_args_parse2(parser, argset),                            \
        _nx_args_parse2_error(_nx_tuple_get(1, checkres))           \
    )

# define _nx_args_check_and_parse2(rules, parser, argset)


# define _nx_args_prepend_prefix_cond_d(d, prefix, res, ...) \
    _nx_args_not_empty(__VA_ARGS__)

# define _nx_args_prepend_prefix_op_d(d, prefix, res, arg, ...) \
    prefix, _nx_tuple_append(res, _nx_concat_3(prefix, _, arg)) _nx_append_va_args(__VA_ARGS__)

# define _nx_args_prepend_prefix_res_d(d, prefix, res, ...) \
    _nx_tuple_unpack(res)

# define _nx_args_prepend_prefix_d(d, prefix, ...)                                  \
    _nx_while_d(d)(                                                                 \
        _nx_args_prepend_prefix_cond_d,                                             \
        _nx_args_prepend_prefix_op_d,                                               \
        _nx_args_prepend_prefix_res_d,                                              \
        prefix, _nx_tuple() _nx_append_va_args(__VA_ARGS__)                         \
    )



/// @param parser macro, which expects `argset` as it's input
# define NX_ARGS_PARSE2(max_arg, parser, ...) \
    _nx_args_parse2(parser, NX_CREATE_ARGSET(max_arg, __VA_ARGS__))


# define NX_ARGS_CHECK_AND_PARSE2(max_arg, rule_set, parser, ...) \
    _nx_args_check_and_parse2(rule_set, parser, NX_CREATE_ARGSET(max_arg, __VA_ARGS__))

# define NX_ARGS_PARSE2_PREFIX(max_arg, parser, prefix, ...) \
    _nx_args_parse2(parser, NX_CREATE_ARGSET(max_arg, _nx_args_prepend_prefix_d(0, prefix _nx_append_va_args(__VA_ARGS__))))

#endif //NX_MACRO_ARGS_PARSE_2_HPP
