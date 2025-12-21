//
// Created by nexie on 08.12.2025.
//

#ifndef ARGNAME_HPP
#define ARGNAME_HPP

#include <nx/macro.hpp>

#include <nx/macro/detail/arg_tk_limit.hpp>
#include <nx/macro/repeating/iterate.hpp>

// #define __NX_TOKENIZE_MACRO_(n, macro, ...) \
//     macro NX_CONCAT(_arg_tk_, NX_CHOOSE(n, , __VA_ARGS__)) )
//
// #define __NX_TOKENIZE_ARGUMENTS_(macro, ...) \
//     NX_REPEAT(NX_ARGS_COUNT(__VA_ARGS__), __NX_TOKENIZE_MACRO_, macro, __VA_ARGS__)
//
// #define __NX_TOKENIZE_ARGUMENTS(...) \
//     __NX_TOKENIZE_ARGUMENTS_(__VA_ARGS__)
//
// #define __NX_X_ARGUMENTS(macro, ...) \
//     __NX_TOKENIZE_ARGUMENTS(macro, __VA_ARGS__)
//
// #define __NX_CHOOSE_ARGUMENT(idx, ...)


#define _nx_arguments_tokenize_iterator(n, macro, d) \
     macro _nx_concat_2(_nx_arg_tk_, d) )

#define _nx_arguments_tokenize(macro, ...) \
    _nx_make_iterate(_nx_arguments_tokenize_iterator, macro, __VA_ARGS__)


/// INPUT(1 123, 2 123, 3 123)


#endif //ARGNAME_HPP
