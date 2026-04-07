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

#if defined(NX_CXX_MSVC)
    #define NX_THREAD_LOCAL static __declspec(thread)
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

#if defined(NX_CXX_CLANG) || defined(NX_CXX_GCC)
    #define NX_FALLTHROUGH [[fallthrough]];
#elif defined(NX_CXX_MSVC)
    #define NX_FALLTHROUGH [[fallthrough]];
#else
    #define NX_FALLTHROUGH
#endif


//====================================================
// UNUSED
//====================================================

#if defined(NX_CXX_GCC) || defined(NX_CXX_CLANG)
    #define NX_UNUSED(x) (void)(x)
#else
    #define NX_UNUSED(x) ((void)(x))
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


#if defined(__has_cpp_attribute)
    #if __has_cpp_attribute(nodiscard)
        #define NX_NODISCARD [[nodiscard]]
    #else
        #define NX_NODISCARD
    #endif

    #if __has_cpp_attribute(maybe_unused)
        #define NX_MAYBE_UNUSED [[maybe_unused]]
    #else
        #define NX_MAYBE_UNUSED
    #endif

    #if __has_cpp_attribute(deprecated)
        #define NX_DEPRECATED [[deprecated]]
        #define NX_DEPRECATED_MSG(msg) [[deprecated(msg)]]
    #else
        #define NX_DEPRECATED
        #define NX_DEPRECATED_MSG(msg)
    #endif

    #if __has_cpp_attribute(noreturn)
        #define NX_NORETURN [[noreturn]]
    #else
        #define NX_NORETURN
    #endif

    #if __has_cpp_attribute(no_unique_address)
        #define NX_NO_UNIQUE_ADDRESS [[no_unique_address]]
    #else
        #define NX_NO_UNIQUE_ADDRESS
    #endif
#else
    #define NX_NODISCARD
    #define NX_MAYBE_UNUSED
    #define NX_DEPRECATED
    #define NX_DEPRECATED_MSG(msg)
    #define NX_NORETURN
    #define NX_NO_UNIQUE_ADDRESS
#endif

#if defined(NX_CXX_MSVC)
    #define NX_ASSUME(expr) __assume(expr)
    #define NX_UNREACHABLE() __assume(0)
    #define NX_DEBUG_BREAK() __debugbreak()
    #define NX_RESTRICT __restrict
#elif defined(NX_CXX_GCC) || defined(NX_CXX_CLANG)
    #define NX_ASSUME(expr) do { if (!(expr)) __builtin_unreachable(); } while (0)
    #define NX_UNREACHABLE() __builtin_unreachable()
    #define NX_DEBUG_BREAK() __builtin_trap()
    #define NX_RESTRICT __restrict__
#else
    #define NX_ASSUME(expr) ((void)0)
    #define NX_UNREACHABLE() ((void)0)
    #define NX_DEBUG_BREAK() ((void)0)
    #define NX_RESTRICT
#endif

#endif //NX_HELPERS_HPP