//
// Created by nexie on 07.12.2025.
//

#ifndef NX_MACRO_LOGIC_HPP
#define NX_MACRO_LOGIC_HPP

#include "platform.hpp"

#define __NX_OR(a, b) __NX_OR_(a, b)
#define __NX_OR_(a, b) NX_CONCAT(__NX_OR_, NX_CONCAT(a, b))
#define __NX_OR_00 0
#define __NX_OR_01 1
#define __NX_OR_10 1
#define __NX_OR_11 1

#define __NX_AND(a, b) __NX_AND_(a, b)
#define __NX_AND_(a, b) NX_CONCAT(__NX_AND_, NX_CONCAT(a, b))
#define __NX_AND_00 0
#define __NX_AND_01 0
#define __NX_AND_10 0
#define __NX_AND_11 1

#define __NX_XOR(a, b) __NX_XOR_(a, b)
#define __NX_XOR_(a, b) NX_CONCAT(__NX_XOR_, NX_CONCAT(a, b))
#define __NX_XOR_00 0
#define __NX_XOR_01 1
#define __NX_XOR_10 1
#define __NX_XOR_11 0

#endif //NX_MACRO_LOGIC_HPP
