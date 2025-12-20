//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_NUMBER_DEC_LIMIT_HPP
#define NX_MACRO_NUMBER_DEC_LIMIT_HPP

#ifndef NX_MAX_NUMBER
#error __FILE__ "NX_MAX_NUMBER is undefined"
#else

#if NX_MAX_NUMBER >= 256
#include "limits/number_dec_256.hpp"
#elif NX_MAX_NUMBER >= 128
#include "limits/number_dec_128.hpp"
#elif NX_MAX_NUMBER >= 64
#include "limits/number_dec_64.hpp"
#endif
#endif

#if NX_MAX_NUMBER != NX_LIMITS_DEC
#error "NX_MAX_NUMBER != NX_LIMITS_INC"
#endif

#endif //NUMBER_DEC_N_HPP
