//
// Created by nexie on 19.02.2026.
//

#ifndef NX_MACRO_ARGS_PARSE_2_HPP
#define NX_MACRO_ARGS_PARSE_2_HPP

#include <nx/macro.hpp>
#include <nx/macro/logic/if.hpp>
#include <nx/macro/logic/bool.hpp>
#include <nx/macro/repeating/iterate.hpp>
#include <nx/macro/args/append.hpp>
#include <nx/macro/args/count.hpp>

#include <nx/macro/repeating/while.hpp>
#include <nx/macro/numeric/compare.hpp>
#include <nx/macro/numeric/sum.hpp>
#include <nx/macro/tuple/tuple.hpp>

#include <nx/macro/args/token.hpp>
#include <nx/macro/args/argset.hpp>



#define PARAM1 1
#define PARAM2 2
#define PARAM3 10

// #define tokens _nx_args_tokenize_all(PARAM1 HELLO, PARAM2 WORLD, PARAM3 333)

// _nx_args_max_token_name_op_d(0, 0, (PARAM1, HELLO), (PARAM2, WORLD))

// _nx_args_max_token_name_d(0, (PARAM1, HELLO), (PARAM2, WORLD), (PARAM3, 333))

// _nx_args_tokenize_all(PARAM1 hello, PARAM2 world, PARAM3 another one)

_nx_args_to_argset_autosize(24 VALUVALUVALUVAL, PARAM1 HELLO, PARAM2 HEY, PARAM3 VALUE)

#endif //NX_MACRO_ARGS_PARSE_2_HPP