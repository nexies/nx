//
// Created by nexie on 20.12.2025.
//

#ifndef ARITHMETIC_SUM_HPP
#define ARITHMETIC_SUM_HPP

#include "../repeat.hpp"
#include "../util/arg_count.hpp"

#define __NX_ARITHMETIC_SUM_2(x, y) \
    NX_ARGS_COUNT(NX_PLACEHOLDERS(x), NX_PLACEHOLDERS(y))

#define NX_ARITHMETIC_SUM(...) \
    __NX_ARITHMETIC_SUM_2(__VA_ARGS__)

#endif //ARITHMETIC_SUM_HPP
