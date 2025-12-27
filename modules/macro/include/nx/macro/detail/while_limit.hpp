//
// Created by nexie on 21.12.2025.
//

#ifndef WHILE_LIMITS_HPP
#define WHILE_LIMITS_HPP

# ifndef NX_MAX_NUMBER
    # error "NX_MAX_NUMBER is undefined"
# endif

# define NX_MACRO_PRIVATE_INCLUDE

# if   NX_MAX_NUMBER >= 16384
    # include <nx/macro/detail/limits/while_16384.hpp>
# elif NX_MAX_NUMBER >= 8192
    # include <nx/macro/detail/limits/while_8192.hpp>
# elif NX_MAX_NUMBER >= 4096
    # include <nx/macro/detail/limits/while_4096.hpp>
# elif NX_MAX_NUMBER >= 2048
    # include <nx/macro/detail/limits/while_2048.hpp>
# elif NX_MAX_NUMBER >= 1024
    # include <nx/macro/detail/limits/while_1024.hpp>
# elif NX_MAX_NUMBER >= 512
    # include <nx/macro/detail/limits/while_512.hpp>
# elif NX_MAX_NUMBER >= 256
    # include <nx/macro/detail/limits/while_256.hpp>
# elif NX_MAX_NUMBER >= 128
    # include <nx/macro/detail/limits/while_128.hpp>
# elif NX_MAX_NUMBER >= 64
    # include <nx/macro/detail/limits/while_64.hpp>
# endif

#undef NX_MACRO_PRIVATE_INCLUDE

#if NX_MAX_NUMBER != NX_LIMITS_WHILE
#error "NX_MAX_NUMBER != NX_LIMITS_INC"
#endif


#endif //WHILE_LIMITS_HPP
