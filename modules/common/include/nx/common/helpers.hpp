//
// Created by nexie on 24.03.2026.
//

#ifndef NX_COMMON_HELPERS_HPP
#define NX_COMMON_HELPERS_HPP

#include <nx/common/platform/platform_defs.hpp>
#include <nx/common/platform/compiler_defs.hpp>

#if defined(NX_CXX_MSC)
    #define NX_FORCE_INLINE __forceinline
    #define NX_NO_INLINE __declspec(noinline)
#elif defined(NX_CXX_GCC) || defined(NX_CXX_CLANG)
    #define NX_FORCE_INLINE inline __attribute__((always_inline))
    #define NX_NO_INLINE __attribute__((noinline))
#else
    #define NX_FORCE_INLINE inline
    #define NX_NO_INLINE
#endif


// ====================================================
// LIKELY / UNLIKELY
// ====================================================

#if defined(NX_CXX_GCC) || defined(NX_CXX_CLANG)
    #define NX_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define NX_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define NX_LIKELY(x)   (x)
    #define NX_UNLIKELY(x) (x)
#endif

// ====================================================
// THREAD_LOCAL
// ====================================================

#if defined(NX_COMP_MSVC)
    #define NX_THREAD_LOCAL __declspec(thread)
#else
    #define NX_THREAD_LOCAL thread_local
#endif

//====================================================
// ENDIANNESS
//====================================================

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define NX_ENDIAN_BIG 1
#else
    #define NX_ENDIAN_LITTLE 1
#endif

//====================================================
// DEBUG / RELEASE
//====================================================

#if defined(NDEBUG)
    #define NX_RELEASE 1
#else
    #define NX_DEBUG 1
#endif

//====================================================
// FALLTHROUGH
//====================================================

#if defined(NX_COMP_CLANG) || defined(NX_COMP_GCC)
    #define NX_FALLTHROUGH [[fallthrough]]
#elif defined(NX_COMP_MSVC)
    #define NX_FALLTHROUGH [[fallthrough]]
#else
    #define NX_FALLTHROUGH
#endif


//====================================================
// UNUSED
//====================================================

#if defined(NX_COMP_GCC) || defined(NX_COMP_CLANG)
    #define NX_UNUSED(x) (void)(x)
#else
    #define NX_UNUSED(x)
#endif


//====================================================
// STATIC ASSERT WRAPPER
//====================================================

#define NX_STATIC_ASSERT(expr, msg) static_assert((expr), msg)


//====================================================
// DISABLE_COPY / DISABLE_MOVE
//====================================================

# define NX_DISABLE_COPY(Class) \
    Class (const Class & ) = delete; \
    Class & operator = (const Class & ) = delete;

# define NX_DISABLE_MOVE(Class) \
    Class (Class && ) = delete; \
    Class & operator = (Class &&) = delete;

#endif //NX_HELPERS_HPP