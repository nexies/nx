//
// Created by nexie on 24.03.2026.
//

#ifndef NX_COMMON_COMPILER_DEFS_HPP
#define NX_COMMON_COMPILER_DEFS_HPP

//--------------------------------------------------------------------------------------------------
// Compiler detection
//--------------------------------------------------------------------------------------------------
/**
 * @name Compiler macros
 *
 * Supported compiler families:
 * - GCC      -> NX_CXX_GCC, NX_CXX_GCC_VER
 * - Clang    -> NX_CXX_CLANG, NX_CXX_LLVM_CLANG_VER / NX_CXX_APPLE_CLANG_VER
 * - Borland  -> NX_CXX_BORLAND
 * - MSVC     -> NX_CXX_MSC, NX_CXX_MSC_VER
 *
 * NX_CXX contains a human-readable compiler name.
 */
//@{

#if defined(_MSC_VER) && defined(__clang__)
#  error "Unsupported compiler configuration"
#endif

#if defined(__GNUC__)
#  define NX_CXX_GCC
#  define NX_CXX "gcc"
#  define NX_CXX_GCC_VER (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#  if NX_CXX_GCC_VER < 40000
#    error "Unsupported GCC version"
#  endif

#elif defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
#  define NX_CXX_BORLAND
#  define NX_CXX "bcb"
#  error "NX_CXX_BORLAND version macro is not specified"

#elif defined(_MSC_VER)
#  define NX_CXX_MSC
#  define NX_CXX_MSVC
#  define NX_CXX "MS VisualC"
#  define NX_CXX_MSC_VER (_MSC_VER)
#  define NX_CXX_MSVC_VER (_MSC_VER)

#else
#  error "Unknown compiler - please reconfigure"
#endif

#if defined(__clang__)
#  define NX_CXX_CLANG
#  if defined(NX_CXX_GCC)
#    undef NX_CXX
#  endif
#  define NX_CXX "clang"
#  if defined(__apple_build_version__)
#    define NX_CXX_APPLE_CLANG_VER (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#  else
#    define NX_CXX_LLVM_CLANG_VER (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#  endif
#endif

//@}

// Compatibility for non-Clang compilers.
#ifndef __has_feature
#  define __has_feature(x) 0
#endif

#define NX_HAS_FEATURE(__feature__) __has_feature(__feature__)

#ifndef __has_builtin
#  define __has_builtin(x) 0
#endif

#define NX_HAS_BUILTIN(__builtin__) __has_builtin(__builtin__)

//--------------------------------------------------------------------------------------------------
// Compiler control helpers
//--------------------------------------------------------------------------------------------------
/**
 * @brief Indicates support for `#pragma GCC diagnostic push/pop`.
 *
 * Available for:
 * - GCC 4.6+
 * - LLVM Clang 1.7+
 * - Apple Clang 2.0+
 */
#if (defined(NX_CXX_GCC_VER) && (NX_CXX_GCC_VER >= 40600)) || \
(defined(NX_CXX_LLVM_CLANG_VER) && (NX_CXX_LLVM_CLANG_VER >= 10700)) || \
(defined(NX_CXX_APPLE_CLANG_VER) && (NX_CXX_APPLE_CLANG_VER >= 20000))
#  define NX_GCC_DIAGNOSTIC_AWARE 1
#endif

#if defined(NX_GCC_DIAGNOSTIC_AWARE)
#  pragma GCC diagnostic ignored "-Wunknown-pragmas"
#  pragma GCC diagnostic ignored "-Wunused-function"
#endif

#endif //NX_COMMON_COMPILER_DEFS_HPP