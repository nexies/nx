//
// Created by nexie on 24.03.2026.
//

#ifndef NX_COMMON_SYMBOLS_DEFS_HPP
#define NX_COMMON_SYMBOLS_DEFS_HPP

//--------------------------------------------------------------------------------------------------
// Symbol visibility and calling conventions
//--------------------------------------------------------------------------------------------------
/**
 * @name Export / import helpers
 *
 * These macros simplify shared library symbol export/import declarations:
 * - NX_SYMBOL_EXPORT
 * - NX_SYMBOL_IMPORT
 *
 * Calling convention helpers:
 * - NX_STDCALL
 * - NX_CDECL
 */
//@{

#include "platform_defs.hpp"

#if defined(__GNUC__) && (__GNUC__ >= 4) && \
(defined(_WIN32) || defined(__WIN32__) || defined(WIN32)) && !defined(__CYGWIN__)
#  define NX_SYMBOL_EXPORT __attribute__((dllexport))
#  define NX_SYMBOL_IMPORT __attribute__((dllimport))
#endif

#if defined(NX_OS_WINDOWS)
#  ifndef NX_SYMBOL_EXPORT
#    define NX_SYMBOL_EXPORT __declspec(dllexport)
#  endif
#  ifndef NX_SYMBOL_IMPORT
#    define NX_SYMBOL_IMPORT __declspec(dllimport)
#  endif
#  define NX_STDCALL __stdcall
#  define NX_CDECL   __cdecl
#else
#  define NX_SYMBOL_EXPORT
#  define NX_SYMBOL_IMPORT
#  define NX_STDCALL
#  define NX_CDECL
#endif

//@}

#endif //NX_COMMON_SYMBOLS_DEFS_HPP