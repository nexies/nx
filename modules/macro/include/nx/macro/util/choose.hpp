//
// Created by nexie on 24.11.2025.
//

#ifndef CHOOSE_HPP
#define CHOOSE_HPP

#include <nx/macro.hpp>
#include <nx/macro/detail/choose_limit.hpp>

#define _nx_choose(pos, ...) \
    NX_CONCAT(_nx_choose_, pos)(__VA_ARGS__)

#define NX_CHOOSE(pos, ...) \
    _nx_choose(pos, __VA_ARGS__)


#define _nx_req_choose_0(c, ...) c
#define _nx_req_choose_1(c, ...) _nx_req_choose_0(__VA_ARGS__)
#define _nx_req_choose_2(c, ...) _nx_req_choose_1(__VA_ARGS__)
#define _nx_req_choose_3(c, ...) _nx_req_choose_2(__VA_ARGS__)
#define _nx_req_choose_4(c, ...) _nx_req_choose_3(__VA_ARGS__)

// _nx_req_choose_2(1, 2, 3, 4, 5)

#endif //CHOOSE_HPP
