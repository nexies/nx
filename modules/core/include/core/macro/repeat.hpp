//
// Created by nexie on 22.11.2025.
//

#ifndef REPEAT_HPP
#define REPEAT_HPP

#include "platform.hpp"

#define NX_REPEAT_MAX_COUNT 32

#define __NX_REPEAT_0(c, h, m, d)
#define __NX_REPEAT_1(c, h, m, d)   __NX_REPEAT_0(0, h, m, d) h(c, m, 1, d)
#define __NX_REPEAT_2(c, h, m, d)   __NX_REPEAT_1(0, h, m, d) h(c, m, 2, d)
#define __NX_REPEAT_3(c, h, m, d)   __NX_REPEAT_2(0, h, m, d) h(c, m, 3, d)
#define __NX_REPEAT_4(c, h, m, d)   __NX_REPEAT_3(0, h, m, d) h(c, m, 4, d)
#define __NX_REPEAT_5(c, h, m, d)   __NX_REPEAT_4(0, h, m, d) h(c, m, 5, d)
#define __NX_REPEAT_6(c, h, m, d)   __NX_REPEAT_5(0, h, m, d) h(c, m, 6, d)
#define __NX_REPEAT_7(c, h, m, d)   __NX_REPEAT_6(0, h, m, d) h(c, m, 7, d)
#define __NX_REPEAT_8(c, h, m, d)   __NX_REPEAT_7(0, h, m, d) h(c, m, 8, d)
#define __NX_REPEAT_9(c, h, m, d)   __NX_REPEAT_8(0, h, m, d) h(c, m, 9, d)
#define __NX_REPEAT_10(c, h, m, d)  __NX_REPEAT_9(0, h, m, d) h(c, m, 10, d)
#define __NX_REPEAT_11(c, h, m, d)  __NX_REPEAT_10(0, h, m, d) h(c, m, 11, d)
#define __NX_REPEAT_12(c, h, m, d)  __NX_REPEAT_11(0, h, m, d) h(c, m, 12, d)
#define __NX_REPEAT_13(c, h, m, d)  __NX_REPEAT_12(0, h, m, d) h(c, m, 13, d)
#define __NX_REPEAT_14(c, h, m, d)  __NX_REPEAT_13(0, h, m, d) h(c, m, 14, d)
#define __NX_REPEAT_15(c, h, m, d)  __NX_REPEAT_14(0, h, m, d) h(c, m, 15, d)
#define __NX_REPEAT_16(c, h, m, d)  __NX_REPEAT_15(0, h, m, d) h(c, m, 16, d)
#define __NX_REPEAT_17(c, h, m, d)  __NX_REPEAT_16(0, h, m, d) h(c, m, 17, d)
#define __NX_REPEAT_18(c, h, m, d)  __NX_REPEAT_17(0, h, m, d) h(c, m, 18, d)
#define __NX_REPEAT_19(c, h, m, d)  __NX_REPEAT_18(0, h, m, d) h(c, m, 19, d)
#define __NX_REPEAT_20(c, h, m, d)  __NX_REPEAT_19(0, h, m, d) h(c, m, 20, d)
#define __NX_REPEAT_21(c, h, m, d)  __NX_REPEAT_20(0, h, m, d) h(c, m, 21, d)
#define __NX_REPEAT_22(c, h, m, d)  __NX_REPEAT_21(0, h, m, d) h(c, m, 22, d)
#define __NX_REPEAT_23(c, h, m, d)  __NX_REPEAT_22(0, h, m, d) h(c, m, 23, d)
#define __NX_REPEAT_24(c, h, m, d)  __NX_REPEAT_23(0, h, m, d) h(c, m, 24, d)
#define __NX_REPEAT_25(c, h, m, d)  __NX_REPEAT_24(0, h, m, d) h(c, m, 25, d)
#define __NX_REPEAT_26(c, h, m, d)  __NX_REPEAT_25(0, h, m, d) h(c, m, 26, d)
#define __NX_REPEAT_27(c, h, m, d)  __NX_REPEAT_26(0, h, m, d) h(c, m, 27, d)
#define __NX_REPEAT_28(c, h, m, d)  __NX_REPEAT_27(0, h, m, d) h(c, m, 28, d)
#define __NX_REPEAT_29(c, h, m, d)  __NX_REPEAT_28(0, h, m, d) h(c, m, 29, d)
#define __NX_REPEAT_30(c, h, m, d)  __NX_REPEAT_29(0, h, m, d) h(c, m, 30, d)
#define __NX_REPEAT_31(c, h, m, d)  __NX_REPEAT_30(0, h, m, d) h(c, m, 31, d)
#define __NX_REPEAT_32(c, h, m, d)  __NX_REPEAT_31(0, h, m, d) h(c, m, 32, d)

#define __NX_REPEAT_WITH_HELPER_(count, helper, macro, data) \
    NX_CONCAT(__NX_REPEAT_, count)(1, helper, macro, data)

#define NX_REPEAT_WITH_HELPER(...) __NX_REPEAT_WITH_HELPER_(__VA_ARGS__)

#define __NX_COMMA_HELPER_0(m, n, d) m(n, d),
#define __NX_COMMA_HELPER_1(m, n, d) m(n, d)
#define __NX_COMMA_HELPER_(c, m, n, d) NX_CONCAT(__NX_COMMA_HELPER_, c)(m, n, d)

#define __NX_REPEAT_(count, macro, data) \
    NX_REPEAT_WITH_HELPER(count, __NX_COMMA_HELPER_, macro, data)

#define NX_REPEAT(...) __NX_REPEAT_(__VA_ARGS__)

// NX_REPEAT(10, TO_NUMBER)

#endif //REPEAT_HPP
