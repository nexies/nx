//
// Created by nexie on 21.12.2025.
//

#ifndef NX_MACRO_SUM_HPP
#define NX_MACRO_SUM_HPP

#include <nx/macro.hpp>

#include <nx/macro/repeating/while.hpp>
#include <nx/macro/numeric/is_max.hpp>
#include <nx/macro/logic/bool.hpp>
#include <nx/macro/logic/op.hpp>

# define _nx_numeric_sum_condition(d, car, res) \
    _nx_logic_and( \
        _nx_bool(car), \
        _nx_logic_not( _nx_is_max(res) ) \
    )

# define _nx_numeric_sum_operation(d, car, res) \
    _nx_dec(car), _nx_inc(res)

# define _nx_numeric_sum_result(d, car, res) \
    res

# define _nx_numeric_sum_d(d, a, b) \
    _nx_while_##d( \
        _nx_numeric_sum_condition, \
        _nx_numeric_sum_operation, \
        _nx_numeric_sum_result, \
        a, b \
    )

# define _nx_numeric_sum(a, b) \
    _nx_numeric_sum_d(0, a, b)

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


/**
 * @brief Performs a preprocessor-level addition of two integer literals with an explicit recursion depth parameter.
 *
 * `NX_NUMERIC_SUM_D` expands to the result of @p a + @p b, similar to NX_NUMERIC_SUM,
 *  additionally takes an explicit depth parameter @p d used to control evaluation
 * inside iterative or recursive preprocessor constructs (e.g., loops and repeated macros).
 *
 * This form is intended for use within NX_WHILE macros that already manage a recursion/depth
 * parameter and therefore cannot rely on the default depth used by NX_NUMERIC_SUM.
 *
 * If the sum exceeds the maximum supported numeric value (`NX_MAX_NUMBER`),
 * the result is clamped to `NX_MAX_NUMBER`.
 *
 * Both operands must be valid numeric literals supported by the numeric
 * subsystem (i.e., within the allowed `NX_MAX_NUMBER` range).
 *
 * @par Example
 * @code
 *
 * #define MY_WHILE_OPERATION(d, a, b) \
        NX_DEC(a), NX_NUMERIC_SUM_D(d, a, b)
 *
 * @endcode
 *
 * @param d Explicit recursion/depth parameter for use inside preprocessor loops.
 * @param a First addend.
 * @param b Second addend.
 *
 * @see NX_WHILE
 * @see NX_NUMERIC_SUM
 * @see NX_NUMERIC_SUB_D
 * @see NX_INC_D
 * @see NX_DEC_D
 */
#define NX_NUMERIC_SUM_D(d, a, b) \
    _nx_numeric_sum_d(d, a, b)



#endif //SUM_HPP
