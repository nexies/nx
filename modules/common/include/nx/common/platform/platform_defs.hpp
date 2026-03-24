//
// Created by nexie on 24.03.2026.
//

#ifndef NX_COMMON_OS_DEFS_HPP
#define NX_COMMON_OS_DEFS_HPP


// MS-compatible compilers support #pragma once.
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#  pragma once
#endif


//--------------------------------------------------------------------------------------------------
// Platform detection
//--------------------------------------------------------------------------------------------------
/**
 * @name Platform macros
 *
 * Exactly one primary platform macro is expected to be defined:
 * - NX_OS_LINUX
 * - NX_OS_BSD
 * - NX_OS_SOLARIS
 * - NX_OS_CYGWIN
 * - NX_OS_WINDOWS
 * - NX_OS_APPLE
 * - NX_OS_QNX
 * - NX_OS_UNIX
 *
 * Additional derived macros may also be defined:
 * - NX_OS_MCBC
 * - NX_OS_ASTRA_LINUX
 * - NX_POSIX
 *
 * @note
 * NX_PLATFORM contains a human-readable platform name.
 *
 * @note
 * Cygwin is treated as a POSIX platform, not as native Windows.
 */
//@{

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
#  define NX_OS_LINUX
#  define NX_POSIX
#  if defined(__mcbc__)
#    define NX_OS_MCBC
#    define NX_PLATFORM "MCBC"
#  elif defined(__astra_linux__)
#    define NX_OS_ASTRA_LINUX
#    define NX_PLATFORM "Astra Linux"
#  else
#    define NX_PLATFORM "Linux"
#  endif


#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#  define NX_OS_BSD
#  define NX_POSIX
#  define NX_PLATFORM "BSD"

#elif defined(sun) || defined(__sun)
#  define NX_OS_SOLARIS
#  define NX_POSIX
#  define NX_PLATFORM "Solaris"

#elif defined(__CYGWIN__)
#  define NX_OS_CYGWIN
#  define NX_POSIX
#  define NX_PLATFORM "Cygwin"

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WINDOWS)
#  define NX_OS_WINDOWS
#  define NX_PLATFORM "Windows"
#  if defined(__MINGW32__)
#    include <_mingw.h>
#  endif

#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#  define NX_OS_APPLE
#  define NX_POSIX
#  define NX_PLATFORM "MacOS"

#elif defined(__QNXNTO__)
#  define NX_OS_QNX
#  define NX_POSIX
#  define NX_PLATFORM "QNX"

#elif defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
#  define NX_OS_UNIX
#  define NX_POSIX
#  define NX_PLATFORM "Unix"

#else
#  error "Unknown platform - please reconfigure"
#endif

//@}

//--------------------------------------------------------------------------------------------------
// Windows include tuning
//--------------------------------------------------------------------------------------------------

#if defined(NX_OS_WINDOWS)
#  if !defined(WIN32_LEAN_AND_MEAN)
#    define WIN32_LEAN_AND_MEAN
#  endif
#endif

#endif //NX_OS_DEFS_HPP