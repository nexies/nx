//
// Created by nexie on 24.11.2025.
//

#ifndef CHOOSE_HPP
#define CHOOSE_HPP

#include <nx/macro.hpp>
#include <nx/macro/detail/choose_limit.hpp>

#define _nx_choose_(pos, ...) \
    NX_CONCAT(_nx_choose_, pos)(__VA_ARGS__)

#define NX_CHOOSE(pos, ...) \
    _nx_choose_(pos, __VA_ARGS__)


#endif //CHOOSE_HPP
