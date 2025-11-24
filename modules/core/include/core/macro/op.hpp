//
// Created by nexie on 23.11.2025.
//

#ifndef OP_HPP
#define OP_HPP

#include "arg_count.hpp"
#include "repeat.hpp"

#define NX_MAKE_ADD(x, y) \
    NX_ARGS_COUNT(NX_PLACEHOLDERS(x), NX_PLACEHOLDERS(y))

#endif //OP_HPP
