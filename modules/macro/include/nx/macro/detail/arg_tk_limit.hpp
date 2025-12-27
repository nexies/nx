//
// Created by nexie on 21.12.2025.
//

#ifndef ARG_TK_LIMITS_HPP
#define ARG_TK_LIMITS_HPP

# ifndef NX_MAX_NUMBER
    # error "NX_MAX_NUMBER is undefined"
# endif

# define NX_MACRO_PRIVATE_INCLUDE

# if   NX_MAX_NUMBER >= 16384
    # include <nx/macro/detail/limits/arg_tk_16384.hpp>
# elif NX_MAX_NUMBER >= 8192
    # include <nx/macro/detail/limits/arg_tk_8192.hpp>
# elif NX_MAX_NUMBER >= 4096
    # include <nx/macro/detail/limits/arg_tk_4096.hpp>
# elif NX_MAX_NUMBER >= 2048
    # include <nx/macro/detail/limits/arg_tk_2048.hpp>
# elif NX_MAX_NUMBER >= 1024
    # include <nx/macro/detail/limits/arg_tk_1024.hpp>
# elif NX_MAX_NUMBER >= 512
    # include <nx/macro/detail/limits/arg_tk_512.hpp>
# elif NX_MAX_NUMBER >= 256
    # include <nx/macro/detail/limits/arg_tk_256.hpp>
# elif NX_MAX_NUMBER >= 128
    # include <nx/macro/detail/limits/arg_tk_128.hpp>
# elif NX_MAX_NUMBER >= 64
    # include <nx/macro/detail/limits/arg_tk_64.hpp>
# endif

#undef NX_MACRO_PRIVATE_INCLUDE

#if NX_MAX_NUMBER != NX_LIMITS_ARG_TK
#error "NX_MAX_NUMBER != NX_LIMITS_ARG_TK"
#endif

#endif //ARG_TK_LIMITS_HPP
