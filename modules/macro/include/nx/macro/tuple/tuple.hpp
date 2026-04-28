//
// Created by nexie on 22.12.2025.
//

#ifndef NXTBOT_TUPLE_HPP
#define NXTBOT_TUPLE_HPP

// #define NX_MACRO_TUPLE_SET_USE_WHILE

# include <nx/macro.hpp>
# include <nx/macro/args/count.hpp>
# include <nx/macro/util/choose.hpp>
# include <nx/macro/args/append.hpp>

# include <nx/macro/logic/bool.hpp>
# include <nx/macro/logic/op.hpp>


# define _nx_tuple(...) (__VA_ARGS__)

# define _nx_tuple_unpack(t) _nx_prescan(_nx_expand t)

# define _nx_tuple_size(t)   _nx_prescan(_nx_args_count t)

# define _nx_tuple_not_empty(t) _nx_bool(_nx_tuple_size(t))

# define _nx_tuple_empty(t)  _nx_logic_not(_nx_tuple_not_empty(t))

# define _nx_tuple_get(idx, t) _nx_choose(idx, _nx_tuple_unpack(t))

# define _nx_tuple_2_get(idx, t) _nx_tuple_get(idx, t)

# define _nx_tuple_append(t, ...) \
    _nx_logic_if(_nx_tuple_empty(t)) \
    (\
        _nx_tuple(__VA_ARGS__), \
        _nx_tuple(_nx_tuple_unpack(t)_nx_append_args(__VA_ARGS__)) \
    )

# define _nx_eat_1(c, ...) __VA_ARGS__

# define _nx_tuple_concat(t1, t2) \
    _nx_tuple_append(t1, _nx_tuple_unpack(t2))

# define _nx_tuple_pop_front(t) \
    _nx_logic_if(_nx_tuple_not_empty(t)) \
    ( \
        _nx_tuple(_nx_eat_1 t), \
        _nx_tuple() \
    )


#ifdef NX_MACRO_TUPLE_SET_USE_WHILE
#include <nx/macro/repeating/while.hpp>

# define _nx_tuple_set_cond(d, run, count, elem, pref, suff) \
    _nx_bool(run)

# define _nx_tuple_set_op(d, run, count, elem, pref, suff) \
    _nx_logic_if(_nx_bool(count)) ( \
        _nx_expand(run, _nx_dec(count), elem, _nx_tuple_append(pref, _nx_tuple_get(0, suff)),                            _nx_tuple( _nx_eat_1 suff ) ), \
        _nx_expand(0,   0,              elem, _nx_tuple_concat(_nx_tuple_append(pref, elem), _nx_tuple(_nx_eat_1 suff)), _nx_tuple()) \
    )

# define _nx_tuple_set_res(d, run, count, elem, pref, suff) \
    pref

# define _nx_tuple_set_d(d, tup, pos, elem) \
    _nx_while_d(d)( \
        _nx_tuple_set_cond, \
        _nx_tuple_set_op, \
        _nx_tuple_set_res, \
        1, pos, elem, (), tup)

# define _nx_tuple_set(tup, pos, elem) \
    _nx_tuple_set_d(0, tup, pos, elem)

#else

#include <nx/macro/util/put_at.hpp>

# define _nx_tuple_set(tup, pos, elem) \
    _nx_tuple(_nx_put_at(pos, elem, _nx_tuple_unpack(tup)))

#endif


///
# define NX_TUPLE(...) \
    _nx_tuple(__VA_ARGS__)


///
/// @param t
# define NX_TUPLE_UNPACK(tuple) \
    _nx_tuple_unpack(tuple)


///
/// @param t
# define NX_TUPLE_SIZE(tuple) \
    _nx_tuple_size(tuple)


///
/// @param t
# define NX_TUPLE_EMPTY(tuple) \
    _nx_tuple_empty(tuple)

///
/// @param t
# define NX_TUPLE_NOT_EMPTY(tuple) \
    _nx_tuple_not_empty(tuple)


///
/// @param t
/// @param pos
# define NX_TUPLE_GET(tuple, pos) \
    _nx_tuple_get(pos, tuple)

///
/// @param tuple
/// @param pos
/// @param elem
#define NX_TUPLE_SET(tuple, pos, elem) \
    _nx_tuple_set(tuple, pos, elem)


#ifdef NX_MACRO_TUPLE_SET_USE_WHILE

    /// @param d
    /// @param tuple
    /// @param pos
    /// @param elem
    #define NX_TUPLE_SET_D(d, tuple, pos, elem) \
    _nx_tuple_set_d(d, tuple, pos, elem)

#endif

///
/// @param tuple
# define NX_TUPLE_APPEND(tuple, ...) \
    _nx_tuple_append(tuple, __VA_ARGS__)

///
/// @param tuple
# define NX_TUPLE_POP_FRONT(tuple) \
    _nx_tuple_pop_front(tuple)

///
/// @param first
/// @param second
# define NX_TUPLE_CONCAT(first, second) \
    _nx_tuple_concat(first, second)

#endif //NXTBOT_TUPLE_HPP