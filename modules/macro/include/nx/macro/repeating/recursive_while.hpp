//
// Created by nexie on 21.12.2025.
//

#ifndef RECURSIVE_WHILE_HPP
#define RECURSIVE_WHILE_HPP

// #include <nx/macro/logic/if.hpp>
// #include <nx/macro/logic/op.hpp>
// #include <nx/macro/logic/bool.hpp>

#include <nx/macro.hpp>
#include <nx/macro/logic.hpp>
#include <nx/macro/numeric/inc_dec.hpp>
#include <nx/macro/util/choose.hpp>

// #include <boost/preprocessor/while.hpp>
// #include <boost/preprocessor/arithmetic/add.hpp>


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
//      s

// BOOST_PP_WHILE_1(p, o, s)
// IF BOOL(p(2, s)):
//      BOOST_PP_WHILE_2(p, o, o(2, s))
// ELSE:
//      s

# define BOOST_PP_WHILE_1_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_2, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(2, s))
# define BOOST_PP_WHILE_2_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_3, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(3, s))
# define BOOST_PP_WHILE_3_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_4, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(4, s))
# define BOOST_PP_WHILE_4_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_5, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(5, s))
# define BOOST_PP_WHILE_5_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_6, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(6, s))
# define BOOST_PP_WHILE_6_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_7, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(7, s))
# define BOOST_PP_WHILE_7_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_8, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(8, s))
# define BOOST_PP_WHILE_8_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_9, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(9, s))
# define BOOST_PP_WHILE_9_C(c, p, o, s) BOOST_PP_IIF(c, BOOST_PP_WHILE_10, s BOOST_PP_TUPLE_EAT_3)(p, o, BOOST_PP_IIF(c, o, BOOST_PP_NIL BOOST_PP_TUPLE_EAT_2)(10, s))


// #define _nx_numeric_mul_op()

// # define _nx_d_while(c, o, r, l, ...)

///
///     _nx_mul(a, b)
///         _nx_while(
///             _mul_cond,  // a1 == 0
///             _mul_op,    // _nx_dec(a1), _nx_sum_d(l, a2, b)    has to return l, b+a2
///             _mul_res,   //
///             a, a, b
///         )
///


# define _nx_2_while_

# define  _nx_2_while_d(d, c, o, r, ...) \
    _nx_concat_2(_nx_2_while_, d)(c, o, r, __VA_ARGS__)

# define _nx_2_while_0(c, o, r, ...)   _nx_logic_if(c(1,  __VA_ARGS__))(_nx_2_while_1(c, o, r, o(1,  __VA_ARGS__)), r(1, __VA_ARGS__))
# define _nx_2_while_1(c, o, r, ...)   _nx_logic_if(c(2,  __VA_ARGS__))(_nx_2_while_2(c, o, r, o(2,  __VA_ARGS__)), r(2, __VA_ARGS__))
# define _nx_2_while_2(c, o, r, ...)   _nx_logic_if(c(3,  __VA_ARGS__))(_nx_2_while_3(c, o, r, o(3,  __VA_ARGS__)), r(3, __VA_ARGS__))
# define _nx_2_while_3(c, o, r, ...)   _nx_logic_if(c(4,  __VA_ARGS__))(_nx_2_while_4(c, o, r, o(4,  __VA_ARGS__)), r(4, __VA_ARGS__))
# define _nx_2_while_4(c, o, r, ...)   _nx_logic_if(c(5,  __VA_ARGS__))(_nx_2_while_5(c, o, r, o(5,  __VA_ARGS__)), r(5, __VA_ARGS__))
# define _nx_2_while_5(c, o, r, ...)   _nx_logic_if(c(6,  __VA_ARGS__))(_nx_2_while_6(c, o, r, o(6,  __VA_ARGS__)), r(6, __VA_ARGS__))
# define _nx_2_while_6(c, o, r, ...)   _nx_logic_if(c(7,  __VA_ARGS__))(_nx_2_while_7(c, o, r, o(7,  __VA_ARGS__)), r(7, __VA_ARGS__))
# define _nx_2_while_7(c, o, r, ...)   _nx_logic_if(c(8,  __VA_ARGS__))(_nx_2_while_8(c, o, r, o(8,  __VA_ARGS__)), r(8, __VA_ARGS__))
# define _nx_2_while_8(c, o, r, ...)   _nx_logic_if(c(9,  __VA_ARGS__))(_nx_2_while_9(c, o, r, o(9,  __VA_ARGS__)), r(9, __VA_ARGS__))
# define _nx_2_while_9(c, o, r, ...)   _nx_logic_if(c(10, __VA_ARGS__))(_nx_2_while_10(c, o, r, o(10, __VA_ARGS__)), r(10, __VA_ARGS__))
# define _nx_2_while_10(c, o, r, ...)  _nx_logic_if(c(11, __VA_ARGS__))(_nx_2_while_11(c, o, r, o(11, __VA_ARGS__)), r(11, __VA_ARGS__))
# define _nx_2_while_11(c, o, r, ...)  _nx_logic_if(c(12, __VA_ARGS__))(_nx_2_while_12(c, o, r, o(12, __VA_ARGS__)), r(12, __VA_ARGS__))
# define _nx_2_while_12(c, o, r, ...)  _nx_logic_if(c(13, __VA_ARGS__))(_nx_2_while_13(c, o, r, o(13, __VA_ARGS__)), r(13, __VA_ARGS__))
# define _nx_2_while_13(c, o, r, ...)  _nx_logic_if(c(14, __VA_ARGS__))(_nx_2_while_14(c, o, r, o(14, __VA_ARGS__)), r(14, __VA_ARGS__))
# define _nx_2_while_14(c, o, r, ...)  _nx_logic_if(c(15, __VA_ARGS__))(_nx_2_while_15(c, o, r, o(15, __VA_ARGS__)), r(15, __VA_ARGS__))
# define _nx_2_while_15(c, o, r, ...)  _nx_logic_if(c(16, __VA_ARGS__))(_nx_2_while_16(c, o, r, o(16, __VA_ARGS__)), r(16, __VA_ARGS__))
# define _nx_2_while_16(c, o, r, ...)  _nx_logic_if(c(17, __VA_ARGS__))(_nx_2_while_17(c, o, r, o(17, __VA_ARGS__)), r(17, __VA_ARGS__))

# define _nx_2_numeric_sum_condition_d(d, car, res) \
    _nx_bool(car)

# define _nx_2_numeric_sum_operation_d(d, car, res) \
    _nx_dec(car), _nx_inc(res)

# define _nx_2_numeric_sum_result_d(d, car, res) \
    d, res

# define _nx_2_numeric_sum_d(d, a, b) \
    _nx_2_while_d(d, \
        _nx_2_numeric_sum_condition_d, \
        _nx_2_numeric_sum_operation_d, \
        _nx_2_numeric_sum_result_d, \
        a, b \
    )

// _nx_2_numeric_sum_d(0, 3, 2)


# define _nx_2_numeric_sum_condition_d(d, car, res) \
    _nx_bool(car)

# define _nx_2_numeric_sum_operation_d(d, car, res) \
    _nx_dec(car), _nx_inc(res)

# define _nx_2_numeric_sum_result_d(d, car, res) \
    d, res

# define _nx_2_numeric_sum_d(d, a, b) \
    _nx_2_while_##d( \
        _nx_2_numeric_sum_condition_d, \
        _nx_2_numeric_sum_operation_d, \
        _nx_2_numeric_sum_result_d, \
        a, b \
    )


// _nx_2_numeric_sum_d(0, 3, 2)
// _nx_choose(0, _nx_expand(_nx_2_numeric_sum_d(d, car, res))), \

# define _nx_2_fibonacci_operation_d(d, car, res) \
    _nx_dec(car), \
    _nx_choose(1, _nx_expand(_nx_2_numeric_sum_d(d, car, res)))

# define _nx_2_fibonacci_condition_d(d, car, res) \
    _nx_bool(car)

# define _nx_2_fibonacci_result_d(d, car, res) \
    res

# define _nx_2_fibonacci(d, n) \
    _nx_2_while_##d( \
        _nx_2_fibonacci_condition_d, \
        _nx_2_fibonacci_operation_d, \
        _nx_2_fibonacci_result_d, \
        n, 0 \
    )

#endif //RECURSIVE_WHILE_HPP
