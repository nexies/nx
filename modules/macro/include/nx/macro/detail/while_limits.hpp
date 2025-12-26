//
// Created by nexie on 21.12.2025.
//

#ifndef WHILE_LIMITS_HPP
#define WHILE_LIMITS_HPP

#ifndef NX_MAX_NUMBER
#error __FILE__ "NX_MAX_NUMBER is undefined"
#else

#define NX_MACRO_PRIVATE_INCLUDE

#if NX_MAX_NUMBER >= 4096
#include <nx/macro/detail/limits/while_4096.hpp>
#elif NX_MAX_NUMBER >= 256
#include "limits/while_256.hpp"
#elif NX_MAX_NUMBER >= 128
#include "limits/while_128.hpp"
#elif NX_MAX_NUMBER >= 64
#include "limits/while_64.hpp"
#endif
#undef NX_MACRO_PRIVATE_INCLUDE

#endif

#if NX_MAX_NUMBER != NX_LIMITS_SEQUENCE
#error "NX_MAX_NUMBER != NX_LIMITS_INC"
#endif


#endif //WHILE_LIMITS_HPP
