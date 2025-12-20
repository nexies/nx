//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_SEQUENCE_HPP
#define NX_MACRO_SEQUENCE_HPP

#include "sequence_impl.hpp"


/// @param count
/// @param decorator
/// @param macro
#define NX_DECORATED_SEQUENCE(count, decorator, macro, ...) \
    _nx_make_sequence_with_decorator(count, decorator, macro, __VA_ARGS__)

///
/// @param count
#define NX_MAKE_SEQ(count) \
    _nx_make_sequence(count, _nx_expand)

///
/// @param count
#define NX_PLACEHOLDERS(count) \
    _nx_make_sequence(count, _nx_rconcat, _)





#endif //NX_MACRO_ITERATE_HPP
