//
// Created by nexie on 08.12.2025.
//

#ifndef ARGNAME_HPP
#define ARGNAME_HPP

#include "platform.hpp"
#include "../repeat.hpp"
#include "choose.hpp"

#define __NX_MIN_ARG_TOKEN_INDEX 1
#define __NX_MAX_ARG_TOKEN_INDEX 32

#define _arg_tk_1 _arg_tk_1_(
#define _arg_tk_2 _arg_tk_2_(
#define _arg_tk_3 _arg_tk_3_(
#define _arg_tk_4 _arg_tk_4_(
#define _arg_tk_5 _arg_tk_5_(
#define _arg_tk_6 _arg_tk_6_(
#define _arg_tk_7 _arg_tk_7_(
#define _arg_tk_8 _arg_tk_8_(
#define _arg_tk_9 _arg_tk_9_(
#define _arg_tk_10 _arg_tk_10_(
#define _arg_tk_11 _arg_tk_11_(
#define _arg_tk_12 _arg_tk_12_(
#define _arg_tk_13 _arg_tk_13_(
#define _arg_tk_14 _arg_tk_14_(
#define _arg_tk_15 _arg_tk_15_(
#define _arg_tk_16 _arg_tk_16_(
#define _arg_tk_17 _arg_tk_17_(
#define _arg_tk_18 _arg_tk_18_(
#define _arg_tk_19 _arg_tk_19_(
#define _arg_tk_20 _arg_tk_20_(
#define _arg_tk_21 _arg_tk_21_(
#define _arg_tk_22 _arg_tk_22_(
#define _arg_tk_23 _arg_tk_23_(
#define _arg_tk_24 _arg_tk_24_(
#define _arg_tk_25 _arg_tk_25_(
#define _arg_tk_26 _arg_tk_26_(
#define _arg_tk_27 _arg_tk_27_(
#define _arg_tk_28 _arg_tk_28_(
#define _arg_tk_29 _arg_tk_29_(
#define _arg_tk_30 _arg_tk_30_(
#define _arg_tk_31 _arg_tk_31_(
#define _arg_tk_32 _arg_tk_32_(

#define _arg_tk_1_(...) (1, __VA_ARGS__)
#define _arg_tk_2_(...) (2, __VA_ARGS__)
#define _arg_tk_3_(...) (3, __VA_ARGS__)
#define _arg_tk_4_(...) (4, __VA_ARGS__)
#define _arg_tk_5_(...) (5, __VA_ARGS__)
#define _arg_tk_6_(...) (6, __VA_ARGS__)
#define _arg_tk_7_(...) (7, __VA_ARGS__)
#define _arg_tk_8_(...) (8, __VA_ARGS__)
#define _arg_tk_9_(...) (9, __VA_ARGS__)
#define _arg_tk_10_(...) (10, __VA_ARGS__)
#define _arg_tk_11_(...) (11, __VA_ARGS__)
#define _arg_tk_12_(...) (12, __VA_ARGS__)
#define _arg_tk_13_(...) (13, __VA_ARGS__)
#define _arg_tk_14_(...) (14, __VA_ARGS__)
#define _arg_tk_15_(...) (15, __VA_ARGS__)
#define _arg_tk_16_(...) (16, __VA_ARGS__)
#define _arg_tk_17_(...) (17, __VA_ARGS__)
#define _arg_tk_18_(...) (18, __VA_ARGS__)
#define _arg_tk_19_(...) (19, __VA_ARGS__)
#define _arg_tk_20_(...) (20, __VA_ARGS__)
#define _arg_tk_21_(...) (21, __VA_ARGS__)
#define _arg_tk_22_(...) (22, __VA_ARGS__)
#define _arg_tk_23_(...) (23, __VA_ARGS__)
#define _arg_tk_24_(...) (24, __VA_ARGS__)
#define _arg_tk_25_(...) (25, __VA_ARGS__)
#define _arg_tk_26_(...) (26, __VA_ARGS__)
#define _arg_tk_27_(...) (27, __VA_ARGS__)
#define _arg_tk_28_(...) (28, __VA_ARGS__)
#define _arg_tk_29_(...) (29, __VA_ARGS__)
#define _arg_tk_30_(...) (30, __VA_ARGS__)
#define _arg_tk_31_(...) (31, __VA_ARGS__)
#define _arg_tk_32_(...) (32, __VA_ARGS__)

#define __NX_TOKENIZE_MACRO_(n, macro, ...) \
    macro NX_CONCAT(_arg_tk_, NX_CHOOSE(n, , __VA_ARGS__)) )

#define __NX_TOKENIZE_ARGUMENTS_(macro, ...) \
    NX_REPEAT(NX_ARGS_COUNT(__VA_ARGS__), __NX_TOKENIZE_MACRO_, macro, __VA_ARGS__)

#define __NX_TOKENIZE_ARGUMENTS(...) \
    __NX_TOKENIZE_ARGUMENTS_(__VA_ARGS__)

#define __NX_X_ARGUMENTS(macro, ...) \
    __NX_TOKENIZE_ARGUMENTS(macro, __VA_ARGS__)

#define __NX_CHOOSE_ARGUMENT(idx, ...)

/// INPUT(1 123, 2 123, 3 123)


#endif //ARGNAME_HPP
