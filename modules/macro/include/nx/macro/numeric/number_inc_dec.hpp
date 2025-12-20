//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_NUMBER_INC_DEC_HPP
#define NX_MACRO_NUMBER_INC_DEC_HPP

#include <nx/macro.hpp>

#include <nx/macro/detail/number_dec_limit.hpp>
#include <nx/macro/detail/number_inc_limit.hpp>

/**
 *  @brief Decrements an integer number.
 *
 *  Expands to a number, which value equals @p n-1
 *
 *  Integer @p n should satisfy the following condition:
 *  @code
 *  0 < n <= NX_MAX_NUMBER
 *  @endcode
 *  @par
 *  Example:
 *  @code
 *  int N = NX_DEC(10); // N = 9;
 *  @endcode
 * @param n number to decrement
 */
#define NX_DEC(n) \
    _nx_dec(n)

/**
 * @brief Increments an integer number
 *
 * Expands to a number, which value equals @p n+1
 *
 * Integer @p n should satisfy the following condition:
 * @code
 * 0 <= n < NX_MAX_NUMBER
 * @endcode
 *
 * @par
 * Example:
 * @code
 * int N = NX_INC(10); // N = 11;
 * @endcode
 *
 * @param n number to increment
 */
#define NX_INC(n) \
    _nx_inc(n)

#endif //NX_MACRO_NUMBER_INC_DEC_HPP
