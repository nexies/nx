//
// Created by nexie on 08.12.2025.
//

#ifndef NX_MACRO_ARGS_PARSE_HPP
#define NX_MACRO_ARGS_PARSE_HPP

#include <nx/macro.hpp>

#include <nx/macro/detail/arg_tk_limit.hpp>
#include <nx/macro/logic/if.hpp>
#include <nx/macro/logic/bool.hpp>
#include <nx/macro/repeating/iterate.hpp>
#include <nx/macro/args/append.hpp>
#include <nx/macro/args/count.hpp>

#include <nx/macro/numeric/compare.hpp>
#include <nx/macro/numeric/sum.hpp>
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

#define _nx_args_tokenize(arg) \
    _nx_concat_2(_nx_arg_tk_, arg) )

#define _nx_args_arg_name(arg) \
    _nx_tuple_get(0, _nx_args_tokenize(arg))

#define _nx_args_arg_name_is_d(d, name, arg) \
    _nx_numeric_eq_d(d, name, _nx_args_arg_name(arg))

#define _nx_args_arg_name_is(name, arg) \
    _nx_args_arg_name_is_d(0, name, arg)

#define _nx_args_arg_value(arg) \
    _nx_tuple_get(1, _nx_args_tokenize(arg))

#define _nx_args_arg_has_value(arg) \
    _nx_args_not_empty(_nx_args_arg_value(arg))

#define _nx_args_token_has_name_d(d, name, tk) \
    _nx_numeric_eq_d(d, name, _nx_tuple_get(0, tk))

#define _nx_args_count_all_cond_d(d, res, name, ...) \
    _nx_bool(_nx_args_count(__VA_ARGS__))

#define _nx_args_count_all_op_d(d, res, name, cur, ...) \
    _nx_numeric_sum_d(d, res, _nx_args_token_has_name_d(d, name, _nx_args_tokenize(cur))), \
    name \
    _nx_append_args(__VA_ARGS__)

#define _nx_args_count_all_res_d(d, res, name, ...) \
    res

#define _nx_args_contains_res_d(d, res, name, ...) \
    _nx_bool(res)


#define _nx_args_count_all_d(d, name, ...) \
    _nx_while_d(d)( \
        _nx_args_count_all_cond_d, \
        _nx_args_count_all_op_d, \
        _nx_args_count_all_res_d, \
        0, name, __VA_ARGS__ \
    )

#define _nx_args_count_all(name, ...) \
    _nx_args_count_all_d(0, name, __VA_ARGS__)

#define _nx_args_contains_d(d, name, ...) \
    _nx_while_d(d)( \
        _nx_args_count_all_cond_d, \
        _nx_args_count_all_op_d, \
        _nx_args_contains_res_d, \
        0, name, __VA_ARGS__ \
    )

#define _nx_args_contains(name, ...) \
    _nx_args_contains_d(0, name, __VA_ARGS__)

#define _nx_args_get_tuple_cond_d(d, out, name, ...) \
    _nx_bool(_nx_args_count(__VA_ARGS__))

#define _nx_args_get_tuple_op_d(d, out, name, cur, ...) \
    _nx_logic_if(_nx_args_arg_name_is_d(d, name, cur)) \
    ( \
        _nx_logic_if(_nx_args_arg_has_value(cur)) \
        ( \
            _nx_tuple_append(out, _nx_args_arg_value(cur)), \
            out \
        ), \
        out \
    ), \
    name, \
    __VA_ARGS__

#define _nx_args_get_tuple_res_d(d, out, ...) \
    out

#define _nx_args_get_values_tuple_d(d, name, ...) \
    _nx_while_d(d)( \
        _nx_args_get_tuple_cond_d, \
        _nx_args_get_tuple_op_d, \
        _nx_args_get_tuple_res_d, \
        _nx_tuple(), name _nx_append_va_args(__VA_ARGS__) \
    )

#define _nx_args_get_values_tuple(name, ...) \
    _nx_args_get_values_tuple_d(0, name _nx_append_va_args(__VA_ARGS__))

#define _nx_args_get_values_d(d, name, ...) \
    _nx_tuple_unpack(_nx_args_get_values_tuple_d(d, name _nx_append_va_args(__VA_ARGS__)))

#define _nx_args_get_values(name, ...) \
    _nx_args_get_values_d(0, name _nx_append_va_args(__VA_ARGS__))

#define _nx_args_get_first_from_tuple_or_empty(t) \
    _nx_logic_if(_nx_tuple_empty(t)) \
    (\
        _nx_empty(), \
        _nx_tuple_get(0, t) \
    )

#define _nx_args_get_value_d(d, name, ...) \
    _nx_args_get_first_from_tuple_or_empty( \
        _nx_args_get_values_tuple_d(d, name _nx_append_va_args(__VA_ARGS__)) \
    )

#define _nx_args_get_value(name, ...) \
    _nx_args_get_value_d(0, name, __VA_ARGS__)


#define _nx_args_check_rules_cond_d(d, res, err, rules, ...) \
    _nx_logic_and(\
        _nx_bool(res), \
        _nx_tuple_not_empty(rules) \
    )

#define _nx_args_check_rules_op_d(d, res, err, rules, ...) \
    _nx_tuple_get(0, _nx_tuple_get(0, rules))(d, __VA_ARGS__), \
    _nx_tuple_get(1, _nx_tuple_get(0, rules)), \
    _nx_tuple_pop_front(rules), \
    __VA_ARGS__

#define _nx_args_check_rules_res_d(d, res, err, rules, ...) \
    _nx_logic_if(_nx_bool(res)) \
    ( \
        _nx_tuple(1), \
        _nx_tuple(0, err) \
    )

#define _nx_args_check_rules_impl_d(d, rules, ...) \
    _nx_while_d(d)( \
        _nx_args_check_rules_cond_d, \
        _nx_args_check_rules_op_d, \
        _nx_args_check_rules_res_d, \
        1, "", rules, __VA_ARGS__ \
    )

#define _nx_args_rule(check, error_string) \
    _nx_tuple(check, error_string)

#define _nx_args_rule_set(...) \
    _nx_tuple(__VA_ARGS__)

#define _nx_args_parse_impl(parser, check_result, ...) \
    _nx_logic_if(_nx_tuple_get(0, check_result)) \
    ( \
        parser(__VA_ARGS__), \
        static_assert(false, _nx_tuple_get(1, check_result)); \
    )

#define _nx_args_no_rules() \
    _nx_tuple()

#define _nx_args_parse(parser, rule_set, ...) \
    _nx_args_parse_impl(parser, _nx_args_check_rules_impl_d(0, rule_set, __VA_ARGS__), __VA_ARGS__)



#endif //NX_MACRO_ARGS_PARSE_HPP
