//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_IF_HPP
#define NX_MACRO_IF_HPP

#include <nx/macro.hpp>

#define _nx_logic_if_1(t, f) t
#define _nx_logic_if_0(t, f) f

#define _nx_logic_if(c) \
    NX_CONCAT(_nx_logic_if_, NX_EXPAND(c))

#define NX_IF(c) _nx_logic_if(c)

#endif //NX_MACRO_IF_HPP
