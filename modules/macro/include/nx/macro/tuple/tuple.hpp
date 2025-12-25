//
// Created by nexie on 22.12.2025.
//

#ifndef NXTBOT_TUPLE_HPP
#define NXTBOT_TUPLE_HPP

# include <nx/macro.hpp>
# include <nx/macro/util/arg_count.hpp>
# include <nx/macro/util/choose.hpp>

# include <nx/macro/logic/bool.hpp>
# include <nx/macro/logic/op.hpp>


# define _nx_tuple(...) (__VA_ARGS__)

# define _nx_tuple_unpack(t) _nx_expand t

# define _nx_tuple_size(t) _nx_args_count t

# define _nx_tuple_not_empty(t) _nx_bool(_nx_tuple_size(t))

# define _nx_tuple_empty(t)  _nx_logic_not(_nx_tuple_not_empty(t))

# define _nx_tuple_get(idx, t) _nx_choose(idx, _nx_tuple_unpack(t))

#endif //NXTBOT_TUPLE_HPP