#ifndef NX_MACRO_MUL_HPP
#define NX_MACRO_MUL_HPP

#include <nx/macro.hpp>

#include <nx/macro/repeating/while.hpp>
#include <nx/macro/logic/bool.hpp>
#include <nx/macro/logic/op.hpp>
#include <nx/macro/numeric/is_max.hpp>
#include <nx/macro/numeric/sum.hpp>
#include <nx/macro/numeric/compare.hpp>

#define _nx_numeric_mul_condition(d, step, add, res) \
    _nx_logic_and( \
        _nx_bool(step), \
        _nx_logic_not(_nx_is_max(res)) \
    )

#define _nx_numeric_mul_operation(d, step, add, res) \
    _nx_dec(step), \
    add, \
    _nx_numeric_sum_d(d, add, res)

#define _nx_numeric_mul_result(d, step, add, res) \
    _nx_logic_if(_nx_bool(step)) \
    (\
        static_assert(false, "Multiplication product exceeds NX_MAX_NUMBER");, \
        res \
    )

#define _nx_numeric_mul_d(d, a, b) \
    _nx_while_d(d)( \
        _nx_numeric_mul_condition, \
        _nx_numeric_mul_operation, \
        _nx_numeric_mul_result, \
        a, b, 0 \
    )

#define _nx_numeric_mul(a, b) \
    _nx_numeric_mul_d(0, a, b)

///
/// @param a
/// @param b
# define NX_NUMERIC_MUL(a, b) \
    _nx_numeric_mul(a, b)


///
/// @param d
/// @param a
/// @param b
# define NX_NUMERIC_MUL_D(d, a, b) \
    _nx_numeric_mul_d(d, a, b)



#endif