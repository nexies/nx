//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_NUMBER_DEC_LIMIT_HPP
#define NX_MACRO_NUMBER_DEC_LIMIT_HPP

# ifndef NX_MAX_NUMBER
    # error "NX_MAX_NUMBER is undefined"
# endif

# define NX_MACRO_PRIVATE_INCLUDE

# if   NX_MAX_NUMBER >= 16384
    # include <nx/macro/detail/limits/dec_16384.hpp>
# elif NX_MAX_NUMBER >= 8192
    # include <nx/macro/detail/limits/dec_8192.hpp>
# elif NX_MAX_NUMBER >= 4096
    # include <nx/macro/detail/limits/dec_4096.hpp>
# elif NX_MAX_NUMBER >= 2048
    # include <nx/macro/detail/limits/dec_2048.hpp>
# elif NX_MAX_NUMBER >= 1024
    # include <nx/macro/detail/limits/dec_1024.hpp>
# elif NX_MAX_NUMBER >= 512
    # include <nx/macro/detail/limits/dec_512.hpp>
# elif NX_MAX_NUMBER >= 256
    # include <nx/macro/detail/limits/dec_256.hpp>
# elif NX_MAX_NUMBER >= 128
    # include <nx/macro/detail/limits/dec_128.hpp>
# elif NX_MAX_NUMBER >= 64
    # include <nx/macro/detail/limits/dec_64.hpp>
# endif

#undef NX_MACRO_PRIVATE_INCLUDE

# if NX_MAX_NUMBER != NX_LIMITS_DEC
    # error "NX_MAX_NUMBER != NX_LIMITS_DEC"
# endif

#endif //NUMBER_DEC_N_HPP
