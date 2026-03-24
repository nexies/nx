//
// Created by nexie on 24.03.2026.
//

#ifndef NX_COMMON_STANDARD_DEFS_HPP
#define NX_COMMON_STANDARD_DEFS_HPP

//====================================================
// 7. C++ STANDARD
//====================================================

#if __cplusplus >= 202302L
    #define NX_CPP23 1
#elif __cplusplus >= 202002L
    #define NX_CPP20 1
#elif __cplusplus >= 201703L
    #define NX_CPP17 1
#elif __cplusplus >= 201402L
    #define NX_CPP14 1
#elif __cplusplus >= 201103L
    #define NX_CPP11 1
#else
    #error "C++11 or higher required"
#endif

#define NX_CPP_VERSION __cplusplus

#endif //NX_STANDARD_DEFS_HPP