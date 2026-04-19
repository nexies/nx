//
// Created by nexie on 19.02.2026.
//

#ifndef NX_ARGSET_HPP
#define NX_ARGSET_HPP

#include <nx/macro.hpp>
#include <nx/macro/tuple/tuple.hpp>

#include <nx/macro/repeat.hpp>
#include <nx/macro/numeric/compare.hpp>
#include <nx/macro/util/choose.hpp>
#include <nx/macro/util/put_at.hpp>
#include <nx/macro/util/overload.hpp>

#include <nx/macro/args/token.hpp>

/// Argset represents a two-dimensional table of arguments produced while
/// parsing macro input with named parameters.
///
/// The first index corresponds to the argument @p NAME, which must be defined
/// as a positive integer in the range [0, @p NX_MAX_NUMBER].
///
/// (
///     ( APPEARANCE_COUNT, ( VALUE1, VALUE2, VALUE3, VALUE4 ... ) )
///     ( APPEARANCE_COUNT, ( VALUE1, VALUE2, VALUE3, VALUE4 ... ) )
///     ( APPEARANCE_COUNT, ( VALUE1, VALUE2, VALUE3, VALUE4 ... ) )
///     ( APPEARANCE_COUNT, ( VALUE1, VALUE2, VALUE3, VALUE4 ... ) )
/// )

# define _nx_argset_entry() \
    NX_TUPLE(0, NX_TUPLE())

# define _nx_argset_entry_get_appearance_count(argset_entry) \
    NX_TUPLE_GET(argset_entry, 0)

// # define _nx_argset_entry_get_positions(argset_entry) \
    NX_TUPLE_GET(argset_entry, 1)

// # define _nx_argset_entry_get_position(argset_entry, idx) \
    NX_TUPLE_GET(_nx_argset_entry_get_positions(argset_entry), idx)

# define _nx_argset_entry_get_values(argset_entry) \
    NX_TUPLE_GET(argset_entry, 1)

# define _nx_argset_entry_get_value(argset_entry, idx) \
    NX_TUPLE_GET(_nx_argset_entry_get_values(argset_entry), idx)

# define _nx_argset_entry_add_value(argset_entry, value) \
    NX_TUPLE( \
        NX_INC(_nx_argset_entry_get_appearance_count(argset_entry)), \
        NX_TUPLE_APPEND(_nx_argset_entry_get_values(argset_entry), value) \
    ) \

# define _nx_argset_make_iterator(n) \
    _nx_argset_entry()

# define _nx_argset_make(size) \
    NX_TUPLE(NX_SEQUENCE(size, _nx_argset_make_iterator))

# define _nx_argset(size) \
    _nx_argset_make(size)

# define _nx_argset_get_entry(argset, name) \
    NX_TUPLE_GET(argset, name)

# define _nx_argset_set_entry(argset, name, entry) \
    NX_TUPLE_SET(argset, name, entry)

# define _nx_argset_count(argset, name) \
    _nx_argset_entry_get_appearance_count(_nx_argset_get_entry(argset, name))

# define _nx_argset_contains(argset, name) \
    NX_BOOL(_nx_argset_count(argset, name))

# define _nx_argset_size(argset) \
    NX_TUPLE_SIZE(argset)

# define _nx_argset_add_overload_3(argset, name, value) \
    _nx_argset_set_entry(argset, name, \
    _nx_argset_entry_add_value(_nx_argset_get_entry(argset, name), value) \
)

# define _nx_argset_add_overload_2(argset, name) \
    _nx_argset_add_overload_3(argset, name, _nx_empty() /*value*/)

# define _nx_argset_add(...) \
    NX_OVERLOAD(_nx_argset_add_overload, __VA_ARGS__)

# define _nx_argset_get_overload_3(argset, name, idx) \
    _nx_argset_entry_get_value(_nx_argset_get_entry(argset, name), idx)

# define _nx_argset_get_overload_2(argset, name) \
    _nx_argset_entry_get_values(_nx_argset_get_entry(argset, name))

# define _nx_argset_get(...) \
    NX_OVERLOAD(_nx_argset_get_overload, __VA_ARGS__)

// # define _nx_argset_get_pos_overload_3(argset, name, idx) \
//     _nx_argset_entry_get_position(_nx_argset_get_entry(argset, name), idx)
//
// # define _nx_argset_get_pos_overload_2(argset, name) \
//     _nx_argset_entry_get_positions(_nx_argset_get_entry(argset, name))
//
// # define _nx_argset_get_pos(...) \
//     NX_OVERLOAD(_nx_argset_get_pos_overload, __VA_ARGS__)

# define args1 _nx_argset(3)
# define args2 _nx_argset_add(args1, 2)
# define args3 _nx_argset_add(args2, 2)
# define args4 _nx_argset_add(args3, 0, false)
# define args5 _nx_argset_add(args4, 2, true)
# define args6 _nx_argset_get(args5, 2)


# define _nx_argset_contains_many(argset, name) \
    NX_BOOL(NX_DEC(_nx_argset_count(argset, name)))

# define _nx_argset_contains_single(argset, name) \
    NX_AND( \
        _nx_argset_contains(argset, name), \
        NX_NOT(_nx_argset_contains_many(argset, name)) \
    )

#include <nx/macro/args/token.hpp>

# define _nx_args_put_tokens_to_argset_cond_d(d, argset, ...) \
    NX_HAS_ARGS(__VA_ARGS__)

# define _nx_args_put_tokens_to_argset_op_d(d, argset, token, ...) \
    _nx_argset_add(argset, NX_TUPLE_UNPACK(token)) \
    NX_APPEND_VA_ARGS(__VA_ARGS__)

# define _nx_args_put_tokens_to_argset_res_d(d, argset, ...) \
    argset

# define _nx_args_put_tokens_to_argset_d(d, argset, ...) \
    _nx_while_d(d)( \
        _nx_args_put_tokens_to_argset_cond_d, \
        _nx_args_put_tokens_to_argset_op_d, \
        _nx_args_put_tokens_to_argset_res_d, \
        argset NX_APPEND_VA_ARGS(__VA_ARGS__) \
    )


# define _nx_args_tokens_to_argset_fixed_size_d(d, size, ...) \
    _nx_args_put_tokens_to_argset_d(d, _nx_argset(size), __VA_ARGS__)


# define _nx_args_tokens_to_argset_auto_size_d(d, ...) \
    _nx_args_tokens_to_argset_fixed_size_d(d, NX_INC(_nx_args_max_token_name_d(d, __VA_ARGS__)), __VA_ARGS__)

# define _nx_args_to_argset_fixed_size_d(d, size, ...) \
    _nx_args_tokens_to_argset_fixed_size_d(d, size, _nx_args_tokenize_all(__VA_ARGS__))

# define _nx_args_to_argset_auto_size_d(d, ...) \
    _nx_args_tokens_to_argset_auto_size_d(d, _nx_args_tokenize_all(__VA_ARGS__))

# define _nx_args_to_argset_fixed_size(size, ...) \
    _nx_args_tokens_to_argset_fixed_size_d(0, size, __VA_ARGS__)

# define _nx_args_to_argset_auto_size(...) \
    _nx_args_to_argset_auto_size_d(0, __VA_ARGS__)

//_nx_args_tokenize_all(1 2, 2 3, 3 4, 4, 4, 4, 4)
//_nx_args_tokens_to_argset_fixed_size_d(0, 10, (1, 1), (2, 2), (3, 3), (3, 3), (3, 3))
//_nx_args_to_argset_fixed_size_d(0, 10, 1 1, 2 2, 3 3, 3 3, 4, 5, 6, 7)

//_nx_args_tokens_to_argset_d(0, _nx_argset(3), (0, 1), (1, 2), (2, 3), (2, 4)) -> ((1,(0),(1)),(1,(1),(2)),(2,(2,3),(3,4)))
//_nx_args_tokens_to_argset_fixed_size_d(0, 3, (0, 1), (1, 2), (2, 3), (2, 4)) -> ((1,(0),(1)),(1,(1),(2)),(2,(2,3),(3,4)))
// _nx_args_tokens_to_argset_auto_size_d(0, (0, 1), (1, 2), (2, 3), (2, 4)) -> ((1,(0),(1)),(1,(1),(2)),(2,(2,3),(3,4)))

// # define _nx_args_tokens_to_argset_fixed_size_d(d, size, ...) \
//     ARGSET SIZE = size

// _nx_argset(13)

//_nx_args_to_argset_auto_size(11, 0, 0, 0, 0, 1 1)
//              |
//              L----> ((1,(0),()),(1,(1),(1)),(1,(2),(2)),(1,(3),(45)),(1,(4),()),(3,(5,6,7),()),(0,(),()),(0,(),()),(0,(),()),(0,(),()),(1,(8),(100)))


// _nx_args_tokens_to_argset_d(d, _nx_argset(size), __VA_ARGS__)
#endif //NX_ARGSET_HPP