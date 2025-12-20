//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_LOGIC_OP_HPP
#define NX_MACRO_LOGIC_OP_HPP

#include <nx/macro.hpp>

#define _nx_logic_or(a, b) _nx_logic_or_(a, b)
#define _nx_logic_or_(a, b) NX_CONCAT(_nx_logic_or_, NX_CONCAT(a, b))
#define _nx_logic_or_00 0
#define _nx_logic_or_01 1
#define _nx_logic_or_10 1
#define _nx_logic_or_11 1

#define NX_OR(a, b) _nx_logic_or(a, b)

#define _nx_logic_and(a, b) _nx_logic_and_(a, b)
#define _nx_logic_and_(a, b) NX_CONCAT(_nx_logic_and_, NX_CONCAT(a, b))
#define _nx_logic_and_00 0
#define _nx_logic_and_01 0
#define _nx_logic_and_10 0
#define _nx_logic_and_11 1

#define NX_AND(a, b) _nx_logic_and(a, b)

#define _nx_logic_xor(a, b) _nx_logic_xor_(a, b)
#define _nx_logic_xor_(a, b) NX_CONCAT(_nx_logic_xor_, NX_CONCAT(a, b))
#define _nx_logic_xor_00 0
#define _nx_logic_xor_01 1
#define _nx_logic_xor_10 1
#define _nx_logic_xor_11 0

#define NX_XOR(a, b) _nx_logic_xor(a, b)

#define _nx_logic_not(a) _nx_logic_not_(a)
#define _nx_logic_not_(a) NX_CONCAT(_nx_logic_not_, a)
#define _nx_logic_not_1 0
#define _nx_logic_not_0 1

#define NX_NOT(a) _nx_logic_not(a)

#define _nx_logic_nand(a, b) _nx_logic_nand_(a, b)
#define _nx_logic_nand_(a, b) NX_EXPAND(_nx_logic_and(_nx_logic_not(a, b)))

#define NX_NAND(a, b) _nx_logic_nand(a, b)

#endif //NX_MACRO_LOGIC_OP_HPP
