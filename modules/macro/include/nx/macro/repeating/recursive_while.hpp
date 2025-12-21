//
// Created by nexie on 21.12.2025.
//

#ifndef RECURSIVE_WHILE_HPP
#define RECURSIVE_WHILE_HPP

#include <nx/macro/logic/if.hpp>
#include <boost/preprocessor/while.hpp>
#include <boost/preprocessor/arithmetic/add.hpp>


# define BOOST_PP_WHILE_0(p, o, s) BOOST_PP_WHILE_0_C(BOOST_PP_BOOL(p(1, s)), p, o, s)
# define BOOST_PP_WHILE_1(p, o, s) BOOST_PP_WHILE_1_C(BOOST_PP_BOOL(p(2, s)), p, o, s)
# define BOOST_PP_WHILE_2(p, o, s) BOOST_PP_WHILE_2_C(BOOST_PP_BOOL(p(3, s)), p, o, s)
# define BOOST_PP_WHILE_3(p, o, s) BOOST_PP_WHILE_3_C(BOOST_PP_BOOL(p(4, s)), p, o, s)
# define BOOST_PP_WHILE_4(p, o, s) BOOST_PP_WHILE_4_C(BOOST_PP_BOOL(p(5, s)), p, o, s)
# define BOOST_PP_WHILE_5(p, o, s) BOOST_PP_WHILE_5_C(BOOST_PP_BOOL(p(6, s)), p, o, s)
# define BOOST_PP_WHILE_6(p, o, s) BOOST_PP_WHILE_6_C(BOOST_PP_BOOL(p(7, s)), p, o, s)
# define BOOST_PP_WHILE_7(p, o, s) BOOST_PP_WHILE_7_C(BOOST_PP_BOOL(p(8, s)), p, o, s)
# define BOOST_PP_WHILE_8(p, o, s) BOOST_PP_WHILE_8_C(BOOST_PP_BOOL(p(9, s)), p, o, s)
# define BOOST_PP_WHILE_9(p, o, s) BOOST_PP_WHILE_9_C(BOOST_PP_BOOL(p(10, s)), p, o, s)

# define BOOST_PP_WHILE_0_C(c, p, o, s)     \
    BOOST_PP_IIF(c,                         \
        BOOST_PP_WHILE_1,                   \
        s BOOST_PP_TUPLE_EAT_3)             \
        (p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(1, s))


// BOOST_PP_WHILE_0(p, o, s)
// IF BOOL(p(1, s)):
//      BOOST_PP_WHILE_1(p, o, o(1, s))
// ELSE:
//      s(1, s)

// BOOST_PP_WHILE_1(p, o, s)
// IF BOOL(p(2, s)):
//      BOOST_PP_WHILE_2(p, o, o(2, s))
// ELSE:
//      s(2, s)


BOOST_PP_ADD_D()

# define BOOST_PP_WHILE_1_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_2, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(2, s))
# define BOOST_PP_WHILE_2_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_3, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(3, s))
# define BOOST_PP_WHILE_3_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_4, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(4, s))
# define BOOST_PP_WHILE_4_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_5, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(5, s))
# define BOOST_PP_WHILE_5_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_6, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(6, s))
# define BOOST_PP_WHILE_6_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_7, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(7, s))
# define BOOST_PP_WHILE_7_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_8, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(8, s))
# define BOOST_PP_WHILE_8_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_9, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(9, s))
# define BOOST_PP_WHILE_9_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_10, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(10, s))


# define _nx_d_while(c, o, r, l, ...)

///
///     _nx_mul(a, b)
///         _nx_while(
///             _mul_cond,  // a1 == 0
///             _mul_op,    // _nx_dec(a1), _nx_sum_d(l, a2, b)    has to return l, b+a2
///             _mul_res,   //
///             a, a, b
///         )
///

#endif //RECURSIVE_WHILE_HPP
