//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_INC_DEC_HPP
#define NX_MACRO_INC_DEC_HPP

#include <nx/macro.hpp>

#include <nx/macro/detail/number_dec_limit.hpp>
#include <nx/macro/detail/number_inc_limit.hpp>

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
