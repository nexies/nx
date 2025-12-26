//
// Created by nexie on 26.12.2025.
//

#ifndef NXTBOT_APPEND_ARGS_HPP
#define NXTBOT_APPEND_ARGS_HPP

#include <nx/macro/util/arg_count.hpp>
#include <nx/macro/logic.hpp>


#define _nx_append_args_0()
#define _nx_append_args_1(...) , __VA_ARGS__

#define _nx_append_args(...) \
    _nx_bool_override(_nx_append_args, _nx_args_count(__VA_ARGS__))(__VA_ARGS__)


/**
 *
 */
#define NX_APPEND_ARGS(...) \
    _nx_append_args(__VA_ARGS__)
#endif //NXTBOT_APPEND_ARGS_HPP