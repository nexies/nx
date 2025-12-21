//
// Created by nexie on 21.12.2025.
//

#ifndef COMPARE_HPP
#define COMPARE_HPP

#include <nx/macro/numeric/sub.hpp>

#define _nx_numeric_eq(a, b) \
    _nx_logic_and\
    (\
        _nx_logic_not(_nx_bool(_nx_numeric_sub(a, b))), \
        _nx_logic_not(_nx_bool(_nx_numeric_sub(b, a))), \
    )

#define _nx_numeric_g(a, b) \
    _nx_bool(_nx_numeric_sub(a, b))

#define _nx_numeric_l(a, b) \
    _nx_numeric_g(b, a)

#define _nx_numeric_geq(a, b) \
    _nx_logic_not(_nx_numeric_l(a, b))

#define _nx_numeric_leq(a, b) \
    _nx_logic_not(_nx_numeric_g(a, b))


/**
 * @brief Expands to `1` if @p a and @p b are equal, otherwise to `0`.
 *
 * `NX_EQUAL` performs a preprocessor-level numeric equality comparison.
 * Both operands must be integer literals supported by the numeric subsystem.
 *
 * @par Example
 * @code
 * #if NX_EQUAL(4, 4) // Expands to: 1
 * // true
 * #endif
 *
 * #if NX_EQUAL(6, 8) // Expands to: 0
 * // false
 * #endif
 * @endcode
 *
 * @param a Left operand.
 * @param b Right operand.
 */
#define NX_EQUAL(a, b) \
    _nx_numeric_eq(a, b)


/**
 * @brief Expands to `1` if @p a is greater than @p b, otherwise to `0`.
 *
 * `NX_GREATER` performs a preprocessor-level numeric comparison.
 * Both operands must be integer literals in the supported range.
 *
 * @par Example
 * @code
 * #if NX_GREATER(7, 3) // Expands to: 1
 * // true
 * #endif
 * #if NX_GREATER(3, 12) // Expands to: 0
 * // false
 * #endif
 * @endcode
 *
 * @param a Left operand.
 * @param b Right operand.
 */
#define NX_GREATER(a, b) \
    _nx_numeric_g(a, b)


/**
 * @brief Expands to `1` if @p a is less than @p b, otherwise to `0`.
 *
 * `NX_LESS` performs a preprocessor-level numeric comparison.
 * Both operands must be integer literals in the supported range.
 *
 * @par Example
 * @code
 * #if NX_LESS(2, 5) // Expands to: 1
 * // true
 * #endif
 * #if NX_LESS(5, 3) // Expands to: 0
 * // false
 * #endif
 * @endcode
 *
 * @param a Left operand.
 * @param b Right operand.
 */
#define NX_LESS(a, b) \
    _nx_numeric_l(a, b)

/**
 * @brief Expands to `1` if @p a is greater than or equal to @p b, otherwise to `0`.
 *
 * `NX_GREATER_OR_EQUAL` performs a preprocessor-level numeric comparison.
 * Both operands must be integer literals in the supported range.
 *
 * @par Example
 * @code
 * #if NX_GREATER_OR_EQUAL(3, 3) // Expands to: 1
 * // true
 * #endif
 * #if NX_GREATER_OR_EQUAL(3, 7) // Expands to: 0
 * // false
 * #endif
 * @endcode
 *
 * @param a Left operand.
 * @param b Right operand.
 */
#define NX_GREATER_OR_EQUAL(a, b) \
    _nx_numeric_geq(a, b)


/**
 * @brief Expands to `1` if @p a is less than or equal to @p b, otherwise to `0`.
 *
 * `NX_LESS_OR_EQUAL` performs a preprocessor-level numeric comparison.
 * Both operands must be integer literals in the supported range.
 * @par Example
 * @code
 * #if NX_LESS_OR_EQUAL(2, 9)
 * // true
 * #endif
 * #if NX_LESS_OR_EQUAL(5, 5)
 * // true
 * #endif
 * #if NX_LESS_OR_EQUAL(6, 1)
 * // false
 * #endif
 * @endcode
 *
 * @param a Left operand.
 * @param b Right operand.
 */
#define NX_LESS_OR_EQUAL(a, b) \
    _nx_numeric_leq(a, b)







#endif //COMPARE_HPP
