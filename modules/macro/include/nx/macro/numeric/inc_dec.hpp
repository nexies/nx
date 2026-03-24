//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_INC_DEC_HPP
#define NX_MACRO_INC_DEC_HPP

#include <nx/macro.hpp>

#include <nx/macro/detail/number_dec_limit.hpp>
#include <nx/macro/detail/number_inc_limit.hpp>

#define _nx_inc(n) \
    _nx_concat_2(_nx_numeric_inc_, n)

#define _nx_dec(n) \
    _nx_concat_2(_nx_numeric_dec_, n)

/**
 * @brief Expands to an integer value equal to @p n - 1.
 *
 * `NX_DEC` performs a preprocessor-level decrement on an integer literal.
 * The value of @p n must fall within the supported numeric range:
 * @code
 * 0 < n <= NX_MAX_NUMBER
 * @endcode
 *
 * @par Example
 * @code
 * int N = NX_DEC(10); // Expands to: 9
 * @endcode
 *
 * @param n Integer literal to decrement.
 */
#define NX_DEC(n) \
    _nx_dec(n)

/**
 * @brief Expands to an integer value equal to @p n + 1.
 *
 * `NX_INC` performs a preprocessor-level increment on an integer literal.
 * The value of @p n must fall within the supported numeric range:
 * @code
 * 0 <= n < NX_MAX_NUMBER
 * @endcode
 *
 * @par Example
 * @code
 * int N = NX_INC(10); // Expands to: 11
 * @endcode
 *
 * @param n Integer literal to increment.
 */
#define NX_INC(n) \
    _nx_inc(n)

#endif //NX_MACRO_INC_DEC_HPP
