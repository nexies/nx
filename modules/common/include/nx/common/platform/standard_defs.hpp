//
// Created by nexie on 24.03.2026.
//

#ifndef NX_COMMON_STANDARD_DEFS_HPP
#define NX_COMMON_STANDARD_DEFS_HPP

//====================================================
// 7. C++ STANDARD
//====================================================

#if defined(_MSC_VER)
    #define NX_CPP_VERSION _MSVC_LANG
#else
    #define NX_CPP_VERSION __cplusplus
#endif

#if NX_CPP_VERSION >= 202302L
    #define NX_CPP23 1
#elif NX_CPP_VERSION >= 202002L
    #define NX_CPP20 1
#elif NX_CPP_VERSION >= 201703L
    #define NX_CPP17 1
#elif NX_CPP_VERSION >= 201402L
    #define NX_CPP14 1
#elif NX_CPP_VERSION >= 201103L
    #define NX_CPP11 1
#else
    #error "C++11 or higher required"
#endif

#endif //NX_STANDARD_DEFS_HPP