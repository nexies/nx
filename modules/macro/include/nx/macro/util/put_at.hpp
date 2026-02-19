//
// Created by nexie on 19.02.2026.
//

#ifndef NX_MACRO_PUT_AT_HPP
#define NX_MACRO_PUT_AT_HPP

#include <nx/macro.hpp>
#include <nx/macro/args/append.hpp>
#include <nx/macro/detail/put_at_limit.hpp>

# define _nx_put_at(pos, val, ...) \
    NX_CONCAT(_nx_macro_put_at_, pos)(val, __VA_ARGS__)

///
/// @param pos
/// @param val
# define NX_PUT_AT(pos, val, ...) \
    _nx_put_at(pos, val, __VA_ARGS__)


#endif //NX_PLACE_HPP