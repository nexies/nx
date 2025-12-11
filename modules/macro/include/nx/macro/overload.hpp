//
// Created by nexie on 11.12.2025.
//

#ifndef NXTBOT_OVERLOAD_HPP
#define NXTBOT_OVERLOAD_HPP

#include "arg_count.hpp"

#define __NX_OVERLOAD(macro, ...) \
    NX_CONCAT(macro##_, NX_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define NX_OVERLOAD(...) __NX_OVERLOAD(__VA_ARGS__)

#endif //NXTBOT_OVERLOAD_HPP