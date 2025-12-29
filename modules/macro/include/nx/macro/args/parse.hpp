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

#include <nx/macro/repeating/while.hpp>
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

#define _nx_args_combine_rule_args(d, arg_tup, ...) \
    d _nx_append_args(_nx_tuple_unpack(arg_tup)) _nx_append_args(__VA_ARGS__)

#define _nx_args_apply_rule(rule, d, ...) \
    _nx_apply(_nx_tuple_get(0, rule), _nx_args_combine_rule_args(d, _nx_tuple_get(1, rule), __VA_ARGS__))

#define _nx_args_get_rule_err_string(rule) \
    _nx_tuple_get(2, rule)

#define _nx_args_apply_rules_cond_d(d, res, err, rules, ...) \
    _nx_logic_and(\
        _nx_bool(res), \
        _nx_tuple_not_empty(rules) \
    )

#define _nx_args_apply_rules_op_d(d, res, err, rules, ...) \
    _nx_args_apply_rule(_nx_tuple_get(0, rules), d, __VA_ARGS__), \
    _nx_args_get_rule_err_string(_nx_tuple_get(0, rules)), \
    _nx_tuple_pop_front(rules), \
    __VA_ARGS__

#define _nx_args_apply_rules_res_d(d, res, err, rules, ...) \
    _nx_logic_if(_nx_bool(res)) \
    ( \
        _nx_tuple(1), \
        _nx_tuple(0, err) \
    )

#define _nx_args_apply_rules_d(d, rules, ...) \
    _nx_while_d(d)( \
        _nx_args_apply_rules_cond_d, \
        _nx_args_apply_rules_op_d, \
        _nx_args_apply_rules_res_d, \
        1, "INVALID ARGUMENTS: Unknown error", rules, __VA_ARGS__ \
    )

#define _nx_args_custom_rule(check, arg_tuple, error_string) \
    _nx_tuple(check, arg_tuple, error_string)

#define _nx_args_rule(check, error_string) \
    _nx_tuple(check, _nx_tuple(), error_string)

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
    _nx_args_parse_impl(parser, _nx_args_apply_rules_d(0, rule_set, __VA_ARGS__), __VA_ARGS__)

#define _nx_args_rule_not_empty_check(d, ...) \
    _nx_args_not_empty(__VA_ARGS__)

#define _nx_args_rule_conflict_check_d(d, name1, name2, ...) \
    _nx_logic_nand( \
        _nx_bool(_nx_args_contains_d(d, name1, __VA_ARGS__)), \
        _nx_bool(_nx_args_contains_d(d, name2, __VA_ARGS__)) \
    )

#define _nx_args_rule_has_value_check_d(d, name, ...) \
    _nx_logic_if(_nx_args_contains_d(d, name, __VA_ARGS__)) ( \
        _nx_numeric_eq_d(d, \
            _nx_args_count_all_d(d, name, __VA_ARGS__), \
            _nx_args_count(_nx_args_get_values_d(d, name, __VA_ARGS__)) \
        ), \
        1 \
    )

#define _nx_args_rule_unique_check_d(d, name, ...) \
    _nx_numeric_leq_d(d, \
        _nx_args_count_all_d(d, name, __VA_ARGS__), \
        1 \
    )

#define _nx_args_rule_unique_value_check_d(d, name, ...) \
    _nx_logic_and( \
        _nx_args_rule_has_value_check_d(d, name, __VA_ARGS__), \
        _nx_args_rule_unique_check_d(d, name, __VA_ARGS__) \
    )

#define _nx_args_rule_is_flag_check_d(d, name, ...) \
    _nx_logic_if(_nx_args_contains_d(d, name, __VA_ARGS__)) ( \
        _nx_logic_not(_nx_bool(_nx_args_count(_nx_args_get_values_d(d, name, __VA_ARGS__)))), \
        1 \
    )

#define _nx_args_rule_unique_flag_check_d(d, name, ...) \
    _nx_logic_and( \
        _nx_args_rule_is_flag_check_d(d, name, __VA_ARGS__), \
        _nx_args_rule_unique_check_d(d, name, __VA_ARGS__) \
    )

/**
 *
 * @param name
 */
#define NX_ARGS_COUNT_ALL(name, ...) \
    _nx_args_count_all(name, __VA_ARGS__)

/**
 *
 * @param d
 * @param name
 */
#define NX_ARGS_COUNT_ALL_D(d, name, ...) \
    _nx_args_count_all_d(d, name, __VA_ARGS__)

/**
 *
 * @param name
 */
#define NX_ARGS_CONTAINS(name, ...) \
    _nx_args_contains(name, __VA_ARGS__)

/**
 *
 * @param d
 * @param name
 */
#define NX_ARGS_CONTAINS_D(d, name, ...) \
    _nx_args_contains_d(d, name, __VA_ARGS__)

/**
 *
 * @param name
 */
#define NX_ARGS_GET_VALUES(name, ...) \
    _nx_args_get_values(name, __VA_ARGS__)

/**
 *
 * @param d
 * @param name
 */
#define NX_ARGS_GET_VALUES_D(d, name, ...) \
    _nx_args_get_values_d(d, name, __VA_ARGS__)

/**
 *
 * @param name
 */
#define NX_ARGS_GET_VALUES_TUPLE(name, ...) \
    _nx_args_get_values_tuple(name, __VA_ARGS__)

/**
 *
 * @param d
 * @param name
 */
#define NX_ARGS_GET_VALUES_TUPLE_D(d, name, ...) \
    _nx_args_get_values_tuple_d(d, name, __VA_ARGS__)

/**
 *
 * @param name
 */
#define NX_ARGS_GET_VALUE(name, ...) \
    _nx_args_get_value(name, __VA_ARGS__)

/**
 *
 * @param name
 */
#define NX_ARGS_GET_VALUE_D(name, ...) \
    _nx_args_get_value_d(name, __VA_ARGS__)

/**
 *
 * @param check
 * @param error_string
 */
#define NX_ARGS_RULE(check, error_string) \
    _nx_args_rule(check, error_string)

/**
 *
 *
 * @param check
 * @param arguments_tuple
 * @param error_string
 */
#define NX_ARGS_CUSTOM_RULE(check, arguments_tuple, error_string) \
    _nx_args_custom_rule(check, arguments_tuple, error_string)

/**
 *
 */
#define NX_ARGS_RULE_SET(...) \
    _nx_args_rule_set(__VA_ARGS__)



#define NX_ARGS_RULE_NOT_EMPTY \
    _nx_args_rule( \
        _nx_args_rule_not_empty_check, \
        "INVALID PARAMETERS: Arguments cannot be empty" \
    )

#define NX_ARGS_RULE_NOT_EMPTY_S(string) \
    _nx_args_rule( \
        _nx_args_rule_not_empty_check, \
        string \
    )

#define NX_ARGS_RULE_MANDATORY(name) \
    _nx_args_custom_rule( \
        _nx_args_contains_d, \
        (name), \
        NX_TO_STRING_FLAT(INVALID PARAMETERS: Arguments must contain parameter #name.)\
    )

#define NX_ARGS_RULE_MANDATORY_S(name, string) \
    _nx_args_custom_rule( \
        _nx_args_contains_d, \
        (name), \
        string \
    )


#define NX_ARGS_RULE_CONFLICT_S(name1, name2, string) \
    _nx_args_custom_rule( \
        _nx_args_rule_conflict_check_d, \
        _nx_tuple(name1, name2), \
        string \
    )

#define NX_ARGS_RULE_CONFLICT(name1, name2) \
    NX_ARGS_RULE_CONFLICT_S(name1, name2, NX_TO_STRING_FLAT(INVALID PARAMETERS: Arguments cannot contain #name1 and #name2 at the same time.))


#define NX_ARGS_RULE_UNIQUE_S(name, string) \
    _nx_args_custom_rule( \
        _nx_args_rule_unique_check_d, \
        _nx_tuple(name), \
        string \
    )

#define NX_ARGS_RULE_UNIQUE(name) \
    NX_ARGS_RULE_UNIQUE_S(name, NX_TO_STRING_FLAT(INVALID PARAMETERS: Argument #name must be unique))

#define NX_ARGS_RULE_HAS_VALUE_S(name, string) \
    _nx_args_custom_rule( \
        _nx_args_rule_has_value_check_d, \
        _nx_tuple(name), \
        string \
    )

#define NX_ARGS_RULE_HAS_VALUE(name) \
    NX_ARGS_RULE_HAS_VALUE_S(name, NX_TO_STRING_FLAT(INVALID PARAMETERS: Argument #name must have a value))


#define NX_ARGS_RULE_UNIQUE_VALUE_S(name, string) \
    _nx_args_custom_rule( \
        _nx_args_rule_unique_value_check_d, \
        _nx_tuple(name), \
        string \
    )

#define NX_ARGS_RULE_UNIQUE_VALUE(name) \
    NX_ARGS_RULE_UNIQUE_VALUE_S(name, NX_TO_STRING_FLAT(INVALID PARAMETERS: Argument #name must be unique and have a value))


#define NX_ARGS_RULE_IS_FLAG_S(name, string) \
    _nx_args_custom_rule( \
        _nx_args_rule_is_flag_check_d, \
        _nx_tuple(name), \
        string \
    )

#define NX_ARGS_RULE_IS_FLAG(name) \
    NX_ARGS_RULE_IS_FLAG_S(name, NX_TO_STRING_FLAT(INVALID PARAMETERS: Argument #name is a flag (must not have a value)))

#define NX_ARGS_RULE_UNIQUE_FLAG_S(name, string) \
    _nx_args_custom_rule( \
        _nx_args_rule_unique_flag_check_d, \
        _nx_tuple(name), \
        string \
    )

#define NX_ARGS_RULE_UNIQUE_FLAG(name) \
    NX_ARGS_RULE_UNIQUE_FLAG_S(name, NX_TO_STRING_FLAT(INVALID PARAMETERS: Argument #name is a unique flag (must appear once and not have a value)))


#define NX_ARGS_NO_RULES() \
    _nx_args_no_rules()

/**
 *
 * @param parser
 * @param rule_set
 */
#define NX_ARGS_PARSE(parser, rule_set, /*args*/ ...)

#endif //NX_MACRO_ARGS_PARSE_HPP
