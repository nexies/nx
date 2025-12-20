//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_NUMBER_INC_LIMIT_HPP
#define NX_MACRO_NUMBER_INC_LIMIT_HPP

#ifndef NX_MAX_NUMBER
#error __FILE__ "NX_MAX_NUMBER is undefined"
#else

#if NX_MAX_NUMBER >= 256
#include "limits/number_inc_256.hpp"
#elif NX_MAX_NUMBER >= 128
#include "limits/number_inc_128.hpp"
#elif NX_MAX_NUMBER >= 64
#include "limits/number_inc_64.hpp"
#endif
#endif

#if NX_MAX_NUMBER != NX_LIMITS_INC
#error "NX_MAX_NUMBER != NX_LIMITS_INC"
#endif

#define _nx_inc(n) \
    _nx_concat_2(_nx_inc_, n)


#endif //NX_MACRO_NUMBER_INC_LIMIT_HPP
