//
// Created by nexie on 20.04.2026.
//

#ifndef NX_MACRO_ARGS_RULES_HPP
#define NX_MACRO_ARGS_RULES_HPP

#include <nx/macro/args/argset.hpp>

// check_macro mush expect arguments (`d`, `user-args`, `argset`) and evaluate to either 1 or 0
// 1 - rule is obeyed
// 0 - rule is broken
# define _nx_args_rule(check_macro, error_desc)                             \
    _nx_tuple(check_macro, /*user arguments*/ _nx_tuple(), error_desc)

# define _nx_args_custom_rule(check_macro, args_tup, error_desc)            \
    _nx_tuple(check_macro, args_tup, error_desc)

# define _nx_args_rule_macro(rule)                                          \
    _nx_tuple_get(0, rule)

# define _nx_args_rule_user_args(rule)                                      \
    _nx_tuple_get(1, rule)

# define _nx_args_rule_desc(rule)                                           \
    _nx_tuple_get(2, rule)


# define _nx_args_check_rules_prepare_args(d, rule, argset)                 \
    _nx_logic_if(_nx_tuple_empty(_nx_args_rule_user_args(rule))) (          \
        _nx_expand(d, argset),                                              \
        _nx_expand(d, _nx_tuple_unpack(_nx_args_rule_user_args(rule)), argset) \
    )


# define _nx_args_check_rule_d(d, rule, argset)                             \
    _nx_apply(_nx_args_rule_macro(rule),                                    \
        _nx_args_check_rules_prepare_args(d, rule, argset))

# define _nx_args_apply_rule(rule, d, argset)                               \
    _nx_args_check_rule_d(d, rule, argset)

// _nx_args_check_rules semantics:
// argset - set of args to test the rules on
// cont - `continue` either to continue the check or not
// broken - tuple with all the descriptions of broken_rules
// ... rules - va_args with rules to check

// decide either to continue checking rules or not
// if cont = 0 - break;
// if rules is empty - break;
# define _nx_args_check_rules_cond_d(d, argset, cont, broken, ...)          \
    _nx_logic_and(_nx_bool(cont), _nx_args_not_empty(__VA_ARGS__))

# ifdef NX_MACRO_ARGS_CHECK_ALL_RULES
# define _nx_args_check_rules_op_continue 1
#else
# define _nx_args_check_rules_op_continue 0
#endif

# define _nx_args_check_rules_op_d(d, argset, cont, broken, rule, ...)      \
    _nx_logic_if( _nx_args_check_rule_d(d, rule, argset) ) (                \
        _nx_expand(argset, 1, broken _nx_append_va_args(__VA_ARGS__)),      \
        _nx_expand(argset, _nx_args_check_rules_op_continue,                \
                   _nx_tuple_append(broken, _nx_args_rule_desc(rule))       \
                   _nx_append_va_args(__VA_ARGS__))                         \
        )

# define _nx_args_check_rules_res_d(d, argset, cont, broken, ...)           \
    _nx_logic_if(_nx_tuple_not_empty(broken)) (                             \
        _nx_tuple(0, broken),                                               \
        _nx_tuple(1, ())                                                    \
    )

# define _nx_args_check_rules_d(d, argset, rule_set)                        \
    _nx_while_d(d)(                                                         \
        _nx_args_check_rules_cond_d,                                        \
        _nx_args_check_rules_op_d,                                          \
        _nx_args_check_rules_res_d,                                         \
        argset, 1, _nx_tuple()                                              \
        _nx_append_va_args(_nx_tuple_unpack(rule_set))                      \
    )

# define _temp_check_argset_contains_1(d, argset) \
    NX_ARGSET_CONTAINS(argset, 1)

# define _temp_rule_argset_contains_1 \
    _nx_args_custom_rule(_temp_check_argset_contains_1, (), ARGS must contain argument 1)


# define _nx_args_rule_check_contains(d, name, argset) \
    NX_ARGSET_CONTAINS(argset, name)

# define _nx_args_rule_contains_parameter(name)        \
    _nx_args_custom_rule(_nx_args_rule_check_contains, _nx_tuple(name), argset)

# define _nx_args_rule_not_empty_check(d, argset) \
    _nx_logic_not(NX_ARGSET_EMPTY(argset))

# define _nx_args_rule_contains_check(d, name, argset) \
    NX_ARGSET_CONTAINS(argset, name)

# define _nx_args_rule_contains_single_check(d, name, argset) \
    NX_ARGSET_CONTAINS_SINGLE(argset, name)

# define _nx_args_rule_contains_many_check(d, name, argset) \
    NX_ARGSET_CONTAINS_MANY(argset, name)

# define _nx_args_rule_has_value_check(d, name, argset) \
    NX_ARGSET_HAS_VALUE(argset, name)

# define _nx_args_rule_is_flag_check(d, name, argset) \
    NX_ARGSET_IS_FLAG(argset, name)


# define NX_ARGS_RULE_CONTAINS_S(name, string) \
    _nx_args_custom_rule( \
        _nx_args_rule_contains_check, \
        _nx_tuple(name), \
        string \
    )

# define NX_ARGS_RULE_CONTAINS(name) \
    NX_ARGS_RULE_CONTAINS_S()

# define NX_ARGS_RULE_SET(...) \
    _nx_tuple(__VA_ARGS__)

# define NX_ARGS_RULE(check_macro, error_string) \
    _nx_args_rule(check_macro, error_string)

# define NX_ARGS_CUSTOM_RULE(check_macro, error_string, ...) \
    _nx_args_custom_rule(check_macro, _nx_tuple(__VA_ARGS__), error_string)

#endif //NX_MACRO_ARGS_RULES_HPP
