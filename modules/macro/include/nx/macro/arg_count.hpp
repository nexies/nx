//
// Created by nexie on 22.11.2025.
//

#ifndef ARG_COUNT_HPP
#define ARG_COUNT_HPP

#include "platform.hpp"

#define __NX_PP_ARG16( \
    _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
    _8,  _9,  _10, _11, _12, _13, _14, _15, ...) _15


// #define __NX_ARGS_HAS_COMMA_SEQ() \
// 1, 1, 1, \
// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
// 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
// 1, 1, 1, 1, 1, 1, 1, 1, 1, 0

#define __NX_PP_HAS_COMMA(...) \
    __NX_PP_ARG16(__VA_ARGS__, \
        1, 1, 1, 1, 1, 1, 1, 1, \
        1, 1, 1, 1, 1, 1, 0)

#define __NX_PP_TRIGGER_PARENTHESIS_(...) ,

#define __NX_PP_PASTE5(_0, _1, _2, _3, _4) _0##_1##_2##_3##_4

#define __NX_PP_IS_EMPTY_CASE_0001 ,

#define __NX_PP__ISEMPTY(_0, _1, _2, _3) \
    __NX_PP_HAS_COMMA( \
    __NX_PP_PASTE5(__NX_PP_IS_EMPTY_CASE_, _0, _1, _2, _3) \
    )

#define __NX_PP_ISEMPTY(...) \
    __NX_PP__ISEMPTY( \
        __NX_PP_HAS_COMMA(__VA_ARGS__), \
        __NX_PP_HAS_COMMA(__NX_PP_TRIGGER_PARENTHESIS_ __VA_ARGS__), \
        __NX_PP_HAS_COMMA(__VA_ARGS__ (/*empty*/)), \
        __NX_PP_HAS_COMMA(__NX_PP_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/)) \
)


#define __NX_ARGS_64_PLACEHOLDERS_(\
    _1, _2, _3, _4, _5, _6, _7, _8, _9, \
    _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, \
    _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, \
    _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, \
    _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, \
    _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, \
    _60, _61, _62, _63, N, ...) N

#define __NX_ARGS_64_PLACEHOLDERS(...) \
    __NX_ARGS_64_PLACEHOLDERS_(__VA_ARGS__)


#define __NX_ARGS_COUNT_RSEQ() \
    63, 62, 61, 60, \
    59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
    49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
    39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
    29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
    19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
    9, 8, 7, 6, 5, 4, 3, 2, 1, 0


#define __NX_ARGS_COUNT_HELPER_0(...) \
    __NX_ARGS_64_PLACEHOLDERS(__VA_ARGS__, __NX_ARGS_COUNT_RSEQ())
#define __NX_ARGS_COUNT_HELPER_1(...) 0

#define NX_ARGS_COUNT(...) \
    NX_CONCAT(__NX_ARGS_COUNT_HELPER_, __NX_PP_ISEMPTY(__VA_ARGS__))(__VA_ARGS__)

#define __NX_HAS_ARGS_1() 0
#define __NX_HAS_ARGS_0() 1

#define __NX_HAS_ARGS_(...) \
    NX_CONCAT(__NX_HAS_ARGS_, __NX_PP_ISEMPTY(__VA_ARGS__))()

#define NX_HAS_ARGS(...) \
    __NX_HAS_ARGS_(__VA_ARGS__)


#endif //ARG_COUNT_HPP
