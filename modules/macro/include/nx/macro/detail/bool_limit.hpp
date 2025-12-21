//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_BOOL_LIMIT_HPP
#define NX_MACRO_BOOL_LIMIT_HPP

#ifndef NX_MAX_NUMBER
#error __FILE__ "NX_MAX_NUMBER is undefined"
#else

#if NX_MAX_NUMBER >= 256
#include "limits/bool_256.hpp"
#elif NX_MAX_NUMBER >= 128
#include "limits/bool_128.hpp"
#elif NX_MAX_NUMBER >= 64
#include "limits/bool_64.hpp"
#endif
#endif

#if NX_MAX_NUMBER != NX_LIMITS_BOOL
#error "NX_MAX_NUMBER != NX_LIMITS_INC"
#endif

#define _nx_bool(a) \
    _nx_concat_2(_nx_bool_, a)

#endif //BOOL_N_HPP
