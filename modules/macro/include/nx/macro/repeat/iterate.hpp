//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_ITERATE_HPP
#define NX_MACRO_ITERATE_HPP

#include <nx/macro.hpp>

#include <nx/macro/detail/sequence_limit.hpp>

#define __NX_REPEAT_WITH_HELPER_(count, helper, macro, ...) \
NX_CONCAT(__NX_REPEAT_, count)(1, helper, macro, __VA_ARGS__)

#define NX_REPEAT_WITH_HELPER(...) __NX_REPEAT_WITH_HELPER_(__VA_ARGS__)

#define __NX_COMMA_HELPER_0(m, n, ...) m(n, __VA_ARGS__),
#define __NX_COMMA_HELPER_1(m, n, ...) m(n, __VA_ARGS__)
#define __NX_COMMA_HELPER_(c, m, n, ...) NX_CONCAT(__NX_COMMA_HELPER_, c)(m, n, __VA_ARGS__)

#define __NX_NO_COMMA_HELPER(c, m, n, ...) m(n, __VA_ARGS__)

#define __NX_REPEAT_(count, macro, ...) \
NX_REPEAT_WITH_HELPER(count, __NX_COMMA_HELPER_, macro, __VA_ARGS__)

/// Allows you to repeat macro execution for N < NX_REPEAT_MAX_COUNT
/// Usage NX_REPEAT(count, YOUR_MACRO, your_data)
///     count - number of repetitions
///     YOUR_MACRO - your macro-parameter you want to repeat.
///     your_data - your custom data for your YOUR_MACRO
///
#define NX_REPEAT(...) __NX_REPEAT_(__VA_ARGS__)


#define __NX_REPEAT_NO_COMMA_(count, macro, ...) \
NX_REPEAT_WITH_HELPER(count, __NX_NO_COMMA_HELPER, macro, __VA_ARGS__)

#define NX_REPEAT_NO_COMMA(...) __NX_REPEAT_NO_COMMA_(__VA_ARGS__)

#define __NX_MAKE_SEQ_MACRO(n) n

#define NX_MAKE_SEQ(count) \
NX_REPEAT(count, __NX_MAKE_SEQ_MACRO,)

#define NX_PLACEHOLDERS(count) \
NX_REPEAT(count, NX_RCONCAT, _)

#define __NX_PLACEHOLDERS_STARTING_WITH_MACRO(n, s) \
/*NX_ARITHMETIC_SUM*/(n, s)

#define NX_PLACEHOLDERS_STARTING_WITH(start, count) \
NX_REPEAT(count, __NX_PLACEHOLDERS_STARTING_WITH_MACRO, start)

#endif //NX_MACRO_ITERATE_HPP
