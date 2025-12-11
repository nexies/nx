//
// Created by nexie on 24.11.2025.
//

#ifndef CHOOSE_HPP
#define CHOOSE_HPP

#include "platform.hpp"

#define __NX_CHOOSE_0(c0, ...) c0
#define __NX_CHOOSE_1(c0, c1, ...) c1
#define __NX_CHOOSE_2(c0, c1, c2, ...) c2
#define __NX_CHOOSE_3(c0, c1, c2, c3, ...) c3
#define __NX_CHOOSE_4(c0, c1, c2, c3, c4, ...) c4
#define __NX_CHOOSE_5(c0, c1, c2, c3, c4, c5, ...) c5
#define __NX_CHOOSE_6(c0, c1, c2, c3, c4, c5, c6, ...) c6
#define __NX_CHOOSE_7(c0, c1, c2, c3, c4, c5, c6, c7, ...) c7
#define __NX_CHOOSE_8(c0, c1, c2, c3, c4, c5, c6, c7, c8, ...) c8
#define __NX_CHOOSE_9(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, ...) c9
#define __NX_CHOOSE_10(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, ...) c10
#define __NX_CHOOSE_11(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, ...) c11
#define __NX_CHOOSE_12(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, ...) c12
#define __NX_CHOOSE_13(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, ...) c13
#define __NX_CHOOSE_14(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, ...) c14
#define __NX_CHOOSE_15(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15 ...) c15

#define __NX_CHOOSE_(pos, ...) \
    NX_CONCAT(__NX_CHOOSE_, pos)(__VA_ARGS__)

#define NX_CHOOSE(pos, ...) \
    __NX_CHOOSE_(pos, __VA_ARGS__)


#endif //CHOOSE_HPP
