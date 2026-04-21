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

/// Argset — two-dimensional table for parsing named macro parameters.
///
/// Each slot at index @p NAME holds an entry:
///   ( APPEARANCE_COUNT, ( VALUE1, VALUE2, ... ) )
///
/// NAME must be a positive integer in [0, NX_MAX_NUMBER).
/// Typical use: define parameter ids as small integers, then call
///   _nx_args_to_argset_fixed_size_d(d, SIZE, args...)
/// to build the table in one pass.
///
/// Entry layout:
///   field 0 — appearance count (0 = not present, >=1 = present)
///   field 1 — tuple of all collected values

# define _nx_argset_entry() \
    NX_TUPLE(0, NX_TUPLE())

# define _nx_argset_entry_get_count(argset_entry) \
    _nx_tuple_get(0, argset_entry)

# define _nx_argset_entry_get_values(argset_entry) \
    NX_TUPLE_GET(argset_entry, 1)

# define _nx_argset_entry_get_value(argset_entry, idx) \
    NX_TUPLE_GET(_nx_argset_entry_get_values(argset_entry), idx)

# define _nx_argset_entry_add_value(argset_entry, value) \
    NX_TUPLE( \
        NX_INC(_nx_argset_entry_get_count(argset_entry)), \
        NX_TUPLE_APPEND(_nx_argset_entry_get_values(argset_entry), value) \
    )

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
    _nx_argset_entry_get_count(_nx_argset_get_entry(argset, name))

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

# define _nx_argset_contains_many(argset, name) \
    NX_BOOL(NX_DEC(_nx_argset_count(argset, name)))

# define _nx_argset_contains_single(argset, name) \
    NX_AND( \
        _nx_argset_contains(argset, name), \
        NX_NOT(_nx_argset_contains_many(argset, name)) \
    )

# define _nx_argset_has_value(argset, name) \
    NX_BOOL(_nx_argset_get(argset, name))

# define _nx_argset_is_flag(argset, name) \
    NX_AND( \
        _nx_argset_contains(argset, name), \
        NX_NOT(_nx_argset_has_value(argset, name)) \
    )

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
    _nx_args_to_argset_fixed_size_d(0, size, __VA_ARGS__)

# define _nx_args_to_argset_auto_size(...) \
    _nx_args_to_argset_auto_size_d(0, __VA_ARGS__)



// Expansion examples (for documentation purposes):
//
// _nx_args_tokens_to_argset_fixed_size_d(0, 3, (0, 1), (1, 2), (2, 3), (2, 4))
//   -> ((1,(1)),(1,(2)),(2,(3,4)))
//
// _nx_args_to_argset_fixed_size_d(0, 10, 1 hello, 2 world, 3 foo, 3 bar)
//   -> ((0,()),(1,(hello)),(1,(world)),(2,(foo,bar)),...)
//
// _nx_args_to_argset_auto_size_d(0, 1 hello, 2 world, 3 foo)
//   -> ((0,()),(1,(hello)),(1,(world)),(1,(foo)))



// ─────────────────────────────────────────────────────────────────────────────
// Public API
// ─────────────────────────────────────────────────────────────────────────────

/// Build a fixed-size argset from raw named arguments (tokenises internally).
/// Preferred — avoids the extra max-scan pass required by the auto-size variant.
///
/// @param size  Number of slots (must cover the highest parameter id used)
/// @param ...   Named arguments: PARAM_ID value, ...
# define NX_CREATE_ARGSET(size, ...) \
    _nx_args_to_argset_fixed_size(size, __VA_ARGS__)

/// Build a fixed-size argset with explicit while-loop depth @p d.
///
/// @param d     Recursion depth (pass 0 at the outermost call site)
/// @param size  Number of slots
/// @param ...   Named arguments: PARAM_ID value, ...
# define NX_CREATE_ARGSET_D(d, size, ...) \
    _nx_args_to_argset_fixed_size_d(d, size, __VA_ARGS__)

/// Build an argset whose size is derived from the maximum parameter id found
/// in the argument list (runs an extra WHILE pass; use NX_CREATE_ARGSET when
/// the size is known at authoring time).
///
/// @param ...  Named arguments: PARAM_ID value, ...
# define NX_CREATE_ARGSET_AUTO(...) \
    _nx_args_to_argset_auto_size(__VA_ARGS__)

/// Build an auto-sized argset with explicit while-loop depth @p d.
///
/// @param d    Recursion depth
/// @param ...  Named arguments: PARAM_ID value, ...
# define NX_CREATE_ARGSET_AUTO_D(d, ...) \
    _nx_args_to_argset_auto_size_d(d, __VA_ARGS__)

/// Number of slots in @p argset.
///
/// @param argset
# define NX_ARGSET_SIZE(argset) \
    _nx_argset_size(argset)

# define NX_ARGSET_EMPTY(argset) \
    _nx_logic_not(NX_BOOL(NX_ARGSET_SIZE(argset)))

# define NX_ARGSET_NOT_EMPTY(argset) \
    NX_BOOL(NX_ARGSET_SIZE(argset))

/// Add a named entry.
/// Two-argument form records a flag (no value); three-argument form records a value.
///
/// @param argset
/// @param name   Numeric parameter id
/// @param value  (optional) value to store
# define NX_ARGSET_ADD(...) \
    _nx_argset_add(__VA_ARGS__)

/// Get the values tuple for @p name, or a single value by index.
///
/// @param argset
/// @param name   Numeric parameter id
/// @param idx    (optional) zero-based index into the values tuple
# define NX_ARGSET_GET(...) \
    _nx_argset_get(__VA_ARGS__)

/// Appearance count for @p name (0 = absent, ≥1 = present).
///
/// @param argset
/// @param name
# define NX_ARGSET_COUNT(argset, name) \
    _nx_argset_count(argset, name)

/// 1 if @p name appears at least once in @p argset, 0 otherwise.
///
/// @param argset
/// @param name
# define NX_ARGSET_CONTAINS(argset, name) \
    _nx_argset_contains(argset, name)

/// 1 if @p name appears exactly once in @p argset, 0 otherwise.
///
/// @param argset
/// @param name
# define NX_ARGSET_CONTAINS_SINGLE(argset, name) \
    _nx_argset_contains_single(argset, name)

/// 1 if @p name appears more than once in @p argset, 0 otherwise.
///
/// @param argset
/// @param name
# define NX_ARGSET_CONTAINS_MANY(argset, name) \
    _nx_argset_contains_many(argset, name)


# define NX_ARGSET_HAS_VALUE(argset, name) \
    _nx_argset_has_value(argset, name)

# define NX_ARGSET_IS_FLAG(argset, name) \
    _nx_argset_is_flag(argset, name)


#endif //NX_ARGSET_HPP