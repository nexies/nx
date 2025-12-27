//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_IF_HPP
#define NX_MACRO_IF_HPP

#include <nx/macro.hpp>

#define _nx_logic_if_1(t, f) t
#define _nx_logic_if_0(t, f) f

#define _nx_logic_if_(c) \
    _nx_concat_2(_nx_logic_if_, c)

#define _nx_logic_if(...) \
    _nx_logic_if_(_nx_expand(__VA_ARGS__))

#define NX_IF(c) _nx_logic_if(c)

#endif //NX_MACRO_IF_HPP
