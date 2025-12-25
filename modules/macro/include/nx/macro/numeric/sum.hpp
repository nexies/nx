//
// Created by nexie on 21.12.2025.
//

#ifndef NX_MACRO_SUM_HPP
#define NX_MACRO_SUM_HPP

#include <nx/macro/util/choose.hpp>
#include <nx/macro/repeating/while.hpp>
#include <nx/macro/detail/is_max_limit.hpp>

#define _nx_numeric_sum_condition(a, b) \
    _nx_logic_and\
    (\
        _nx_bool(a), \
        _nx_logic_not(_nx_is_max(b)) \
    )

#define _nx_numeric_sum_operation(a, b) \
    _nx_dec(a), _nx_inc(b)

#define _nx_numeric_sum_result(a, b) b

#define _nx_numeric_sum(a, b) \
    _nx_while\
    (\
        _nx_numeric_sum_condition, \
        _nx_numeric_sum_operation, \
        _nx_numeric_sum_result, \
        a, b \
    )


#define _nx_numeric_sum_condition_d(d, a, b) \
    _nx_numeric_sum_condition(a, b)

#define _nx_numeric_sum_operation_d(d, a, b) \
    _nx_inc(d), _nx_dec(a), _nx_inc(b)

#define _nx_numeric_sum_result_d(d, a, b) \
    d, b

#define _nx_numeric_sum_enter_d(d, a, b) \
    _nx_while_##d \
    (\
        _nx_numeric_sum_condition_d, \
        _nx_numeric_sum_operation_d, \
        _nx_numeric_sum_result_d, \
        d, a, b \
    )

#define _nx_numeric_sum_d(d, a, b) \
    _nx_choose(1, _nx_numeric_sum_enter_d(d, a, b))

/**
 * @brief Performs a preprocessor-level addition of two integer literals.
 *
 * `NX_NUMERIC_SUM` expands to the result of @p a + @p b.
 * If the sum exceeds the maximum supported numeric value (`NX_MAX_NUMBER`),
 * the result is clamped to `NX_MAX_NUMBER`.
 *
 * Both operands must be valid numeric literals supported by the numeric
 * subsystem (i.e., within the allowed `NX_MAX_NUMBER` range).
 *
 * @par Example
 * @code
 * int x = NX_NUMERIC_SUM(4, 7);     // Expands to: 11
 * int y = NX_NUMERIC_SUM(100, 300); // Expands to: NX_MAX_NUMBER (if overflow)
 * @endcode
 *
 * @param a First addend.
 * @param b Second addend.
 *
 * @see NX_NUMERIC_SUB
 * @see NX_INC
 * @see NX_DEC
 */
#define NX_NUMERIC_SUM(a, b) \
    _nx_numeric_sum(a, b)


#define NX_NUMERIC_SUM_D(d, a, b) \
    _nx_numeric_sum_enter_d(d, a, b)


#define _nx_numeric_mul_condition_d(d, counter, a, res) \
    _nx_bool(counter)

#define _nx_numeric_mul_operation_d(d, counter, a, res) \
    _nx_choose(0, _nx_numeric_sum_enter_d(d, a, res)), \
    _nx_dec(counter), \
    a, \
    _nx_choose(1, _nx_numeric_sum_enter_d(d, a, res))

#define _nx_numeric_mul_result_d(d, counter, a, res) \
    d, res

#define _nx_numeric_mul_enter_d(d, a, b) \
    _nx_while_##d \
    ( \
        _nx_numeric_mul_condition_d, \
        _nx_numeric_mul_operation_d, \
        _nx_numeric_mul_result_d, \
        d, a, b, 0 \
    )


_nx_numeric_mul_enter_d(0, 1, 1)

#endif //SUM_HPP
