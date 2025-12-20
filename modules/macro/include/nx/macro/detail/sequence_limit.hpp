//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_SEQUENCE_LIMIT_HPP
#define NX_MACRO_SEQUENCE_LIMIT_HPP

#ifndef NX_MAX_NUMBER
#error __FILE__ "NX_MAX_NUMBER is undefined"
#else

#if NX_MAX_NUMBER >= 256
#include "limits/repeat_256.hpp"
#elif NX_MAX_NUMBER >= 128
#include "limits/repeat_128.hpp"
#elif NX_MAX_NUMBER >= 64
#include "limits/repeat_64.hpp"
#endif
#endif

#if NX_MAX_NUMBER != NX_LIMITS_ITERATE
#error "NX_MAX_NUMBER != NX_LIMITS_INC"
#endif

#endif //NX_MACRO_SEQUENCE_LIMIT_HPP
