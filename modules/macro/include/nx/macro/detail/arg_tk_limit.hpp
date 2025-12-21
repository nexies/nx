//
// Created by nexie on 21.12.2025.
//

#ifndef ARG_TK_LIMITS_HPP
#define ARG_TK_LIMITS_HPP

#ifndef NX_MAX_NUMBER
#error __FILE__ "NX_MAX_NUMBER is undefined"
#else

#if NX_MAX_NUMBER >= 256
#include "limits/arg_tk_256.hpp"
#elif NX_MAX_NUMBER >= 128
#include "limits/arg_tk_128.hpp"
#elif NX_MAX_NUMBER >= 64
#include "limits/arg_tk_64.hpp"
#endif
#endif

#if NX_MAX_NUMBER != NX_LIMITS_ARG_TK
#error "NX_MAX_NUMBER != NX_LIMITS_INC"
#endif


#endif //ARG_TK_LIMITS_HPP
