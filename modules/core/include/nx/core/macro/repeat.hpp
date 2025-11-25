//
// Created by nexie on 22.11.2025.
//

#ifndef REPEAT_HPP
#define REPEAT_HPP

#include "platform.hpp"

#define NX_REPEAT_MAX_COUNT 32

#define __NX_REPEAT_0(c, h, m, ...)
#define __NX_REPEAT_1(c, h, m, ...)   __NX_REPEAT_0(0, h, m, __VA_ARGS__) h(c, m, 1, __VA_ARGS__)
#define __NX_REPEAT_2(c, h, m, ...)   __NX_REPEAT_1(0, h, m, __VA_ARGS__) h(c, m, 2, __VA_ARGS__)
#define __NX_REPEAT_3(c, h, m, ...)   __NX_REPEAT_2(0, h, m, __VA_ARGS__) h(c, m, 3, __VA_ARGS__)
#define __NX_REPEAT_4(c, h, m, ...)   __NX_REPEAT_3(0, h, m, __VA_ARGS__) h(c, m, 4, __VA_ARGS__)
#define __NX_REPEAT_5(c, h, m, ...)   __NX_REPEAT_4(0, h, m, __VA_ARGS__) h(c, m, 5, __VA_ARGS__)
#define __NX_REPEAT_6(c, h, m, ...)   __NX_REPEAT_5(0, h, m, __VA_ARGS__) h(c, m, 6, __VA_ARGS__)
#define __NX_REPEAT_7(c, h, m, ...)   __NX_REPEAT_6(0, h, m, __VA_ARGS__) h(c, m, 7, __VA_ARGS__)
#define __NX_REPEAT_8(c, h, m, ...)   __NX_REPEAT_7(0, h, m, __VA_ARGS__) h(c, m, 8, __VA_ARGS__)
#define __NX_REPEAT_9(c, h, m, ...)   __NX_REPEAT_8(0, h, m, __VA_ARGS__) h(c, m, 9, __VA_ARGS__)
#define __NX_REPEAT_10(c, h, m, ...)  __NX_REPEAT_9(0, h, m, __VA_ARGS__) h(c, m, 10, __VA_ARGS__)
#define __NX_REPEAT_11(c, h, m, ...)  __NX_REPEAT_10(0, h, m, __VA_ARGS__) h(c, m, 11, __VA_ARGS__)
#define __NX_REPEAT_12(c, h, m, ...)  __NX_REPEAT_11(0, h, m, __VA_ARGS__) h(c, m, 12, __VA_ARGS__)
#define __NX_REPEAT_13(c, h, m, ...)  __NX_REPEAT_12(0, h, m, __VA_ARGS__) h(c, m, 13, __VA_ARGS__)
#define __NX_REPEAT_14(c, h, m, ...)  __NX_REPEAT_13(0, h, m, __VA_ARGS__) h(c, m, 14, __VA_ARGS__)
#define __NX_REPEAT_15(c, h, m, ...)  __NX_REPEAT_14(0, h, m, __VA_ARGS__) h(c, m, 15, __VA_ARGS__)
#define __NX_REPEAT_16(c, h, m, ...)  __NX_REPEAT_15(0, h, m, __VA_ARGS__) h(c, m, 16, __VA_ARGS__)
#define __NX_REPEAT_17(c, h, m, ...)  __NX_REPEAT_16(0, h, m, __VA_ARGS__) h(c, m, 17, __VA_ARGS__)
#define __NX_REPEAT_18(c, h, m, ...)  __NX_REPEAT_17(0, h, m, __VA_ARGS__) h(c, m, 18, __VA_ARGS__)
#define __NX_REPEAT_19(c, h, m, ...)  __NX_REPEAT_18(0, h, m, __VA_ARGS__) h(c, m, 19, __VA_ARGS__)
#define __NX_REPEAT_20(c, h, m, ...)  __NX_REPEAT_19(0, h, m, __VA_ARGS__) h(c, m, 20, __VA_ARGS__)
#define __NX_REPEAT_21(c, h, m, ...)  __NX_REPEAT_20(0, h, m, __VA_ARGS__) h(c, m, 21, __VA_ARGS__)
#define __NX_REPEAT_22(c, h, m, ...)  __NX_REPEAT_21(0, h, m, __VA_ARGS__) h(c, m, 22, __VA_ARGS__)
#define __NX_REPEAT_23(c, h, m, ...)  __NX_REPEAT_22(0, h, m, __VA_ARGS__) h(c, m, 23, __VA_ARGS__)
#define __NX_REPEAT_24(c, h, m, ...)  __NX_REPEAT_23(0, h, m, __VA_ARGS__) h(c, m, 24, __VA_ARGS__)
#define __NX_REPEAT_25(c, h, m, ...)  __NX_REPEAT_24(0, h, m, __VA_ARGS__) h(c, m, 25, __VA_ARGS__)
#define __NX_REPEAT_26(c, h, m, ...)  __NX_REPEAT_25(0, h, m, __VA_ARGS__) h(c, m, 26, __VA_ARGS__)
#define __NX_REPEAT_27(c, h, m, ...)  __NX_REPEAT_26(0, h, m, __VA_ARGS__) h(c, m, 27, __VA_ARGS__)
#define __NX_REPEAT_28(c, h, m, ...)  __NX_REPEAT_27(0, h, m, __VA_ARGS__) h(c, m, 28, __VA_ARGS__)
#define __NX_REPEAT_29(c, h, m, ...)  __NX_REPEAT_28(0, h, m, __VA_ARGS__) h(c, m, 29, __VA_ARGS__)
#define __NX_REPEAT_30(c, h, m, ...)  __NX_REPEAT_29(0, h, m, __VA_ARGS__) h(c, m, 30, __VA_ARGS__)
#define __NX_REPEAT_31(c, h, m, ...)  __NX_REPEAT_30(0, h, m, __VA_ARGS__) h(c, m, 31, __VA_ARGS__)
#define __NX_REPEAT_32(c, h, m, ...)  __NX_REPEAT_31(0, h, m, __VA_ARGS__) h(c, m, 32, __VA_ARGS__)

#define __NX_REPEAT_WITH_HELPER_(count, helper, macro, ...) \
    NX_CONCAT(__NX_REPEAT_, count)(1, helper, macro, __VA_ARGS__)

#define NX_REPEAT_WITH_HELPER(...) __NX_REPEAT_WITH_HELPER_(__VA_ARGS__)

#define __NX_COMMA_HELPER_0(m, n, ...) m(n, __VA_ARGS__),
#define __NX_COMMA_HELPER_1(m, n, ...) m(n, __VA_ARGS__)
#define __NX_COMMA_HELPER_(c, m, n, ...) NX_CONCAT(__NX_COMMA_HELPER_, c)(m, n, __VA_ARGS__)

#define __NX_REPEAT_(count, macro, ...) \
    NX_REPEAT_WITH_HELPER(count, __NX_COMMA_HELPER_, macro, __VA_ARGS__)

/// Allows you to repeat macro execution for N < NX_REPEAT_MAX_COUNT
/// Usage NX_REPEAT(count, YOUR_MACRO, your_data)
///     count - number of repetitions
///     YOUR_MACRO - your macro-parameter you want to repeat.
///     your_data - your custom data for your YOUR_MACRO
///
#define NX_REPEAT(...) __NX_REPEAT_(__VA_ARGS__)

/// Generate sequences like _1, _2, _3, _4, ... _n
#define NX_PLACEHOLDERS(count) \
    NX_REPEAT(count, NX_RCONCAT, _)

#define __NX_MAKE_SEQ_MACRO(n) n

#define NX_MAKE_SEQ(count) \
    NX_REPEAT(count, __NX_MAKE_SEQ_MACRO,)

#define NX_ARITHMETIC_SUM(x, y) \
    NX_ARGS_COUNT(NX_PLACEHOLDERS(x), NX_PLACEHOLDERS(y))

// NX_REPEAT(10, TO_NUMBER)

#endif //REPEAT_HPP
