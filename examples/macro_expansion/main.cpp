# define _nx_concat_2(a, b) a##b

#define _nx_choose_0(c0, ...) c0
#define _nx_choose_1(c0, c1, ...) c1
#define _nx_choose_2(c0, c1, c2, ...) c2
#define _nx_choose_3(c0, c1, c2, c3, ...) c3
#define _nx_choose_4(c0, c1, c2, c3, c4, ...) c4
#define _nx_choose_5(c0, c1, c2, c3, c4, c5, ...) c5
#define _nx_choose_6(c0, c1, c2, c3, c4, c5, c6, ...) c6
#define _nx_choose_7(c0, c1, c2, c3, c4, c5, c6, c7, ...) c7
#define _nx_choose_8(c0, c1, c2, c3, c4, c5, c6, c7, c8, ...) c8
#define _nx_choose_9(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, ...) c9
#define _nx_choose_10(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, ...) c10
#define _nx_choose_11(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, ...) c11
#define _nx_choose_12(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, ...) c12
#define _nx_choose_13(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, ...) c13
#define _nx_choose_14(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, ...) c14
#define _nx_choose_15(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, ...) c15
#define _nx_choose_16(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, ...) c16

#define _nx_choose(n, ...) \
    _nx_choose_##n(__VA_ARGS__)

#define _nx_inc_0 1
#define _nx_inc_1 2
#define _nx_inc_2 3
#define _nx_inc_3 4
#define _nx_inc_4 5
#define _nx_inc_5 6
#define _nx_inc_6 7
#define _nx_inc_7 8
#define _nx_inc_8 9
#define _nx_inc_9 10
#define _nx_inc_10 11
#define _nx_inc_11 12
#define _nx_inc_12 13
#define _nx_inc_13 14
#define _nx_inc_14 15
#define _nx_inc_15 16
#define _nx_inc_16 17
#define _nx_inc_17 18
#define _nx_inc_18 19
#define _nx_inc_19 20
#define _nx_inc_20 21
#define _nx_inc_21 22
#define _nx_inc_22 23
#define _nx_inc_23 24
#define _nx_inc_24 25

#define _nx_inc(n) _nx_inc##_##n

#define _nx_dec_1 0
#define _nx_dec_2 1
#define _nx_dec_3 2
#define _nx_dec_4 3
#define _nx_dec_5 4
#define _nx_dec_6 5
#define _nx_dec_7 6
#define _nx_dec_8 7
#define _nx_dec_9 8
#define _nx_dec_10 9
#define _nx_dec_11 10
#define _nx_dec_12 11
#define _nx_dec_13 12
#define _nx_dec_14 13
#define _nx_dec_15 14
#define _nx_dec_16 15
#define _nx_dec_17 16
#define _nx_dec_18 17
#define _nx_dec_19 18
#define _nx_dec_20 19
#define _nx_dec_21 20
#define _nx_dec_22 21
#define _nx_dec_23 22
#define _nx_dec_24 23
#define _nx_dec_25 24

#define _nx_dec(n) _nx_dec##_##n

#define _nx_bool_0 0
#define _nx_bool_1 1
#define _nx_bool_2 1
#define _nx_bool_3 1
#define _nx_bool_4 1
#define _nx_bool_5 1
#define _nx_bool_6 1
#define _nx_bool_7 1
#define _nx_bool_8 1
#define _nx_bool_9 1
#define _nx_bool_10 1
#define _nx_bool_11 1
#define _nx_bool_12 1
#define _nx_bool_13 1
#define _nx_bool_14 1
#define _nx_bool_15 1
#define _nx_bool_16 1
#define _nx_bool_17 1
#define _nx_bool_18 1
#define _nx_bool_19 1
#define _nx_bool_20 1
#define _nx_bool_21 1
#define _nx_bool_22 1
#define _nx_bool_23 1
#define _nx_bool_24 1
#define _nx_bool_25 1

#define _nx_bool(n) _nx_bool_##n

#define _nx_expand(...) __VA_ARGS__

#define _nx_logic_if_1(t, f) t
#define _nx_logic_if_0(t, f) f

#define NX_CONCAT(...) \
    _nx_concat_2(__VA_ARGS__)

#define _nx_logic_if(c) \
    NX_CONCAT(_nx_expand(_nx_logic_if_, c))

# define  _nx_2_while_d(d, c, o, r, ...) \
    NX_CONCAT(_nx_2_while_, d)(c, o, r, __VA_ARGS__)

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


// # define _nx_2_while_0_C(p, c, o, r, ...)
//     _nx_logic_if(p)(
//         _nx_logic_if(p)
//         (
//                 _nx_2_while_1(c, o, r, o(1, __VA_ARGS__)),
//                 _nx_pp_nil_
//         ),
//         r(1, __VA_ARGS__)
//     )

# define _nx_2_numeric_sum_condition_d(d, car, res) \
    _nx_bool(car)

# define _nx_2_numeric_sum_operation_d(d, car, res) \
    _nx_dec(car), _nx_inc(res)

# define _nx_2_numeric_sum_result_d(d, car, res) \
    res

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
    _nx_2_numeric_sum_d(d, car, res)
    // _nx_choose(0, _nx_expand(_nx_2_numeric_sum_d(d, car, res)))

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


// # define _check_double_if(num) \
//     _nx_logic_if(_nx_bool(num)) \
//     (\
//         _nx_logic_if(_nx_bool(num)) \
//         ( \
//             true, \
//             __NX_MACRO_PP_NIL__ \
//         ), \
//         pre-false \
//     )

// _check_double_if(10)
_nx_2_fibonacci(0, 4)

// _nx_2_numeric_sum_d(0, 2, 2)
