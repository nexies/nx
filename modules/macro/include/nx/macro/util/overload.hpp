//
// Created by nexie on 11.12.2025.
//

#ifndef NXTBOT_OVERLOAD_HPP
#define NXTBOT_OVERLOAD_HPP

#include <nx/macro.hpp>
#include <nx/macro/args/count.hpp>

#define _nx_overload_impl(macro, count, ...) \
    _nx_concat_3(macro, _, count)(__VA_ARGS__)

#define _nx_overload(macro, ...) \
    _nx_overload_impl(macro, NX_ARGS_COUNT(__VA_ARGS__), __VA_ARGS__)

#define NX_OVERLOAD(...) _nx_expand(_nx_overload(__VA_ARGS__))

#endif //NXTBOT_OVERLOAD_HPP