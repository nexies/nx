//
// Created by nexie on 27.12.2025.
//

#ifndef IS_MAX_HPP
#define IS_MAX_HPP

#include <nx/macro.hpp>
#include <nx/macro/detail/is_max_limit.hpp>

#define _nx_is_max(a) \
    _nx_concat_2(_nx_numeric_is_max_, a)

/**
 *
 * @param a
 */
#define NX_IS_MAX(a) \
    _nx_is_max(a)

#endif //IS_MAX_HPP
