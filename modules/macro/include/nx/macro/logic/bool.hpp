//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_BOOL_HPP
#define NX_MACRO_BOOL_HPP

#include <nx/macro.hpp>
#include <nx/macro/detail/bool_limit.hpp>


#define NX_BOOL(n) \
    _nx_bool(n)


#define _nx_bool_override_p(macro, bit) \
    _nx_concat_3(macro, _, bit)

#define _nx_bool_override(macro, cond) \
    _nx_bool_override_p(macro, _nx_bool(cond))

#endif //NX_MACRO_BOOL_HPP
