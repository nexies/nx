//
// Created by nexie on 22.12.2025.
//

#ifndef NXTBOT_TUPLE_HPP
#define NXTBOT_TUPLE_HPP

# include <nx/macro.hpp>
# include <nx/macro/args/count.hpp>
# include <nx/macro/util/choose.hpp>
# include <nx/macro/args/append.hpp>

# include <nx/macro/logic/bool.hpp>
# include <nx/macro/logic/op.hpp>


# define _nx_tuple(...) (__VA_ARGS__)

# define _nx_tuple_unpack(t) _nx_expand t

# define _nx_tuple_size(t) _nx_args_count t

# define _nx_tuple_not_empty(t) _nx_bool(_nx_tuple_size(t))

# define _nx_tuple_empty(t)  _nx_logic_not(_nx_tuple_not_empty(t))

# define _nx_tuple_get(idx, t) _nx_choose(idx, _nx_tuple_unpack(t))

# define _nx_tuple_append(t, ...) \
    _nx_logic_if(_nx_tuple_empty(t)) \
    (\
        _nx_tuple(__VA_ARGS__), \
        _nx_tuple(_nx_tuple_unpack(t) _nx_append_args(__VA_ARGS__)) \
    )

# define _nx_eat_1(c, ...) __VA_ARGS__

# define _nx_tuple_concat(t1, t2) \
    _nx_tuple_append(t1, _nx_tuple_unpack(t2))

# define _nx_tuple_pop_front(t) \
    _nx_logic_if(_nx_tuple_not_empty(t)) \
    ( \
        _nx_tuple(_nx_eat_1(_nx_tuple_unpack(t))), \
        _nx_tuple() \
    )

#endif //NXTBOT_TUPLE_HPP