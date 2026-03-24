//
// Created by nexie on 24.03.2026.
//

#ifndef NX_COMMON_STD_HEADERS_HPP
#define NX_COMMON_STD_HEADERS_HPP

#ifdef __cplusplus
#  include <cassert>
#  include <cstdlib>
#  include <cstddef>
#else
#  include <assert.h>
#  include <stdlib.h>
#  include <stddef.h>
#endif

// Borland places some memory-related declarations in a non-standard header.
#if defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
#  include <mem.h>
#endif

#if defined(__osf__)
// Tru64 does not provide stdint.h, but inttypes.h contains a compatible superset.
#  include <inttypes.h>
#else
#  include <stdint.h>
#endif

#ifdef max
#  undef max
#endif

#ifdef min
#  undef min
#endif

#endif //NX_COMMON_STD_HEADERS_HPP