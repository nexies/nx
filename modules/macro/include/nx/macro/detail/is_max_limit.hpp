//
// Created by nexie on 21.12.2025.
//

#ifndef NX_MACRO_IS_MAX_LIMIT_HPP
#define NX_MACRO_IS_MAX_LIMIT_HPP

#if NX_MAX_NUMBER >= 256
#include "limits/is_max_256.hpp"
#elif NX_MAX_NUMBER >= 128
#include "limits/is_max_128.hpp"
#elif NX_MAX_NUMBER >= 64
#include "limits/is_max_64.hpp"
#endif

#if NX_MAX_NUMBER != NX_LIMITS_IS_MAX
#error "NX_MAX_NUMBER != NX_LIMITS_INC"
#endif

#define _nx_is_max(a) \
    _nx_concat_2(_nx_is_max_, a)

#endif //IS_MAX_LIMITS_HPP
