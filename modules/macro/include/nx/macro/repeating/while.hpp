//
// Created by nexie on 21.12.2025.
//

#ifndef NX_MACRO_WHILE_HPP
#define NX_MACRO_WHILE_HPP

#include <nx/macro/logic/if.hpp>
#include <nx/macro/detail/while_limits.hpp>
// #include <nx/macro/detail/limits/while_4096.hpp>
#include <nx/macro/detail/limits/while_4096.hpp>

#define _nx_while(c, o, r, ...) \
    _nx_while_0(c, o, r, __VA_ARGS__)

// RECURSION:
// #define _nx_while_d(d, c, o, r, ...) \
    _nx_while_##d (c, o, r, __VA_ARGS__)

/**
 *
 *
 * @param condition
 * @param operation
 * @param result
 */
#define NX_WHILE(condition, operation, result, ...) \
    _nx_while(condition, operation, result, __VA_ARGS__)

#endif //WHILE_HPP
