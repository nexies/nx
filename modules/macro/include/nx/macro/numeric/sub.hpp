//
// Created by nexie on 21.12.2025.
//

#ifndef NX_MACRO_SUB_HPP
#define NX_MACRO_SUB_HPP

#include <nx/macro/repeating/while.hpp>
#include <nx/macro/logic/bool.hpp>
#include <nx/macro/logic/op.hpp>
#include <nx/macro/numeric/inc_dec.hpp>

#define _nx_numeric_sub_condition(a, b) \
    _nx_logic_and(_nx_bool(a), _nx_bool(b))

#define _nx_numeric_sub_operation(a, b) \
    _nx_dec(a), _nx_dec(b)

#define _nx_numeric_sub_result(a, b) a

#define _nx_numeric_sub(a, b) \
    _nx_while\
    (\
        _nx_numeric_sub_condition, \
        _nx_numeric_sub_operation, \
        _nx_numeric_sub_result, \
        a, b \
    )

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

#endif //NX_MACRO_SUB_HPP
