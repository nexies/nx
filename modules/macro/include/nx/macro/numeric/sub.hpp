//
// Created by nexie on 21.12.2025.
//

#ifndef NX_MACRO_SUB_HPP
#define NX_MACRO_SUB_HPP

#include <nx/macro/repeating/while.hpp>
#include <nx/macro/logic/bool.hpp>
#include <nx/macro/logic/op.hpp>
#include <nx/macro/numeric/inc_dec.hpp>

#define _nx_numeric_sub_condition(d, a, b) \
    _nx_logic_and(_nx_bool(a), _nx_bool(b))

#define _nx_numeric_sub_operation(d, a, b) \
    _nx_dec(a), _nx_dec(b)

#define _nx_numeric_sub_result(d, a, b) \
    a

#define _nx_numeric_sub_d(d, a, b) \
    _nx_while_d(d, \
        _nx_numeric_sub_condition, \
        _nx_numeric_sub_operation, \
        _nx_numeric_sub_result, \
        a, b \
    )

#define _nx_numeric_sub(a, b) \
    _nx_numeric_sub_d(0, a, b)

/**
 * @brief Performs a preprocessor-level subtraction of two integer literals.
 *
 * `NX_NUMERIC_SUB` expands to the result of @p a − @p b.
 *
 * If @p b is greater than @p a, the result evaluates to `0`.
 *
 * Both operands must be valid numeric literals supported by the
 * numeric subsystem (i.e., within the allowed `NX_MAX_NUMBER` range).
 *
 * @par Example
 * @code
 * int x = NX_NUMERIC_SUB(10, 3); // Expands to: 7
 * int y = NX_NUMERIC_SUB(3, 10); // Expands to: 0
 * @endcode
 *
 * @param a Minuend — value to subtract from.
 * @param b Subtrahend — value to subtract.
 *
 * @see NX_INC
 * @see NX_DEC
 * @see NX_NUMERIC_ADD
 */
#define NX_NUMERIC_SUB(a, b) \
    _nx_numeric_sub(a, b)

/**
 * @brief Performs a preprocessor-level subtraction of two integer literals with an explicit recursion depth parameter.
 *
 * `NX_NUMERIC_SUB_D` expands to the result of @p a − @p b, similar to ::NX_NUMERIC_SUB,
 * but additionally takes an explicit depth parameter @p d used to control evaluation
 * inside iterative or recursive preprocessor constructs (e.g., loops and repeated macros).
 *
 * This form is intended for use within NX_WHILE-based macros that already manage a
 * recursion/depth parameter and therefore cannot rely on the default depth used by
 * ::NX_NUMERIC_SUB.
 *
 * If the subtraction result becomes less than zero, it is clamped to zero.
 * If the result exceeds the maximum supported numeric value (`NX_MAX_NUMBER`),
 * it is clamped to `NX_MAX_NUMBER`.
 *
 * Both operands must be valid numeric literals supported by the numeric subsystem
 * (i.e., within the allowed `NX_MAX_NUMBER` range).
 *
 * @par Example
 * @code
 * #define MY_WHILE_OPERATION(d, a, b) \
        NX_DEC(a), NX_NUMERIC_SUB_D(d, a, b)
 *
 * @endcode
 *
 * @param d Explicit recursion/depth parameter for use inside preprocessor loops.
 * @param a Minuend (value from which @p b is subtracted).
 * @param b Subtrahend (value being subtracted from @p a).
 *
 * @see NX_WHILE
 * @see NX_NUMERIC_SUB
 * @see NX_NUMERIC_SUM_D
 * @see NX_INC_D
 * @see NX_DEC_D
 */
#define NX_NUMERIC_SUB_D(d, a, b) \
    _nx_numeric_sub_d(d, a, b)

#endif //NX_MACRO_SUB_HPP
