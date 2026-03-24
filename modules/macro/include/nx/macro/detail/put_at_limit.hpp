//
// Created by nexie on 19.02.2026.
//

#ifndef NX_MACRO_PUT_LIMIT_HPP
#define NX_MACRO_PUT_LIMIT_HPP

# ifndef NX_MAX_NUMBER
    # error "NX_MAX_NUMBER is undefined"
# endif

# define NX_MACRO_PRIVATE_INCLUDE

# if   NX_MAX_NUMBER >= 16384
    # include <nx/macro/detail/limits/put_at_16384.hpp>
# elif NX_MAX_NUMBER >= 8192
    # include <nx/macro/detail/limits/put_at_8192.hpp>
# elif NX_MAX_NUMBER >= 4096
    # include <nx/macro/detail/limits/put_at_4096.hpp>
# elif NX_MAX_NUMBER >= 2048
    # include <nx/macro/detail/limits/put_at_2048.hpp>
# elif NX_MAX_NUMBER >= 1024
    # include <nx/macro/detail/limits/put_at_1024.hpp>
# elif NX_MAX_NUMBER >= 512
    # include <nx/macro/detail/limits/put_at_512.hpp>
# elif NX_MAX_NUMBER >= 256
    # include <nx/macro/detail/limits/put_at_256.hpp>
# elif NX_MAX_NUMBER >= 128
    # include <nx/macro/detail/limits/put_at_128.hpp>
# elif NX_MAX_NUMBER >= 64
    # include <nx/macro/detail/limits/put_at_64.hpp>
# endif

#undef NX_MACRO_PRIVATE_INCLUDE

#if NX_MAX_NUMBER != NX_LIMITS_CHOOSE
#error "NX_MAX_NUMBER != NX_LIMITS_CHOOSE"
#endif

#endif //NX_PUT_LIMIT_HPP