//
// Created by nexie on 20.04.2026.
//

#ifndef NX_MACRO_ARGS_RULES_HPP
#define NX_MACRO_ARGS_RULES_HPP

#include <nx/macro/args/argset.hpp>

// check_macro mush expect `argset` and evaluate to either 1 or 0
// 1 - rule is obeyed
// 0 - rule is broken
# define _nx_args_rule(check_macro, error_desc)                             \
    _nx_tuple(check_macro, _nx_tuple(), error_desc)

# define _nx_args_apply_rule_expand_args(rule, argset)                      \
    _nx_tuple_unpack(_nx_tuple_append(_nx_tuple_get(1, rule), argset))

# define _nx_args_apply_rule(rule, d, argset)                               \
    _nx_apply(_nx_tuple_get(0, rule), d                                     \
              _nx_args_apply_rule_expand_args(rule, argset))

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
    _nx_logic_if( _nx_apply(_nx_tuple_get(0, rule), argset) ) (             \
        _nx_expand(argset, 1, broken _nx_append_va_args(__VA_ARGS__)),      \
        _nx_expand(argset, _nx_args_check_rules_op_continue,                \
                   _nx_tuple_append(broken, _nx_tuple_get(2, rule))         \
                   _nx_append_va_args(__VA_ARGS__))                         \
        )

# define _nx_args_check_rules_res_d(d, argset, cont, broken, ...)           \
    _nx_logic_if(_nx_tuple_not_empty(broken)) (                             \
        _nx_tuple(0, broken),                                               \
        _nx_tuple(1, ())                                                    \
    )

# define _nx_args_check_rules_d(d, argset, ...)                             \
    _nx_while_d(d)(                                                         \
        _nx_args_check_rules_cond_d,                                        \
        _nx_args_check_rules_op_d,                                          \
        _nx_args_check_rules_res_d,                                         \
        argset, 1, _nx_tuple() _nx_append_va_args(__VA_ARGS__)              \
    )



#endif //NX_MACRO_ARGS_RULES_HPP
