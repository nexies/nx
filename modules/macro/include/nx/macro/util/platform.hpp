//
// Created by nexie on 22.11.2025.
//

#ifndef PLATFORM_HPP
#define PLATFORM_HPP

// #include <nx/macro/args/count.hpp>

#define _nx_expand(...) __VA_ARGS__
#define NX_EXPAND(...) __VA_ARGS__

#define _nx_expand_args(...) ( __VA_ARGS__ )

// Identity macro used as an extra indirection layer for MSVC/MinGW.
// When a function-like macro M and a parenthesised argument list (a,b,c)
// arrive as separate tokens after argument substitution, MSVC does not
// re-scan "M (a,b,c)" as a call.  Wrapping in _nx_prescan forces the
// inner expression to be evaluated in argument-prescan context, where
// all preprocessors do recognise function-like macro calls.
//
// Usage:  _nx_prescan(SOME_MACRO t)   where t = (a,b,c)
#define _nx_prescan(x) x

#define NX_EVAL(x) x

#define _nx_concat_1(x) x
#define _nx_concat_2(x, y) x##y
#define _nx_concat_3(a, b, c) a##b##c
#define _nx_concat_4(a, b, c, d) a##b##c##d
#define _nx_concat_5(a, b, c, d, e) a##b##c##d##e

#define NX_CONCAT(...) _nx_concat_2(__VA_ARGS__)
#define NX_CONCAT_3(...) _nx_concat_3(__VA_ARGS__)
#define NX_CONCAT_4(...) _nx_concat_4(__VA_ARGS__)
#define NX_CONCAT_5(...) _nx_concat_5(__VA_ARGS__)

#define _nx_rconcat(x, y) y##x
#define NX_RCONCAT(...) _nx_rconcat(__VA_ARGS__)

#define _nx_consume(...)
#define NX_CONSUME(...)  _nx_consume(__VA_ARGS__)

#define __NX_TO_STRING_(v) #v
#define NX_TO_STRING(...) __NX_TO_STRING_(__VA_ARGS__)
#define NX_TO_STRING_FLAT(v) #v

#define _nx_apply(macro, ...) macro(__VA_ARGS__)
#define NX_APPLY(...) _nx_apply(__VA_ARGS__)

#define _nx_empty()
#define NX_EMPTY() _nx_empty()

//#define NX_DEFER(c) c _nx_empty()
#define NX_DEFER(...) __VA_ARGS__ _nx_empty()



#endif //PLATFORM_HPP
