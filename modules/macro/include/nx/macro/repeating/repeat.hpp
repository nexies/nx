//
// Created by nexie on 20.12.2025.
//

#ifndef NX_MACRO_SEQUENCE_REPEAT_HPP
#define NX_MACRO_SEQUENCE_REPEAT_HPP

#include <nx/macro/repeating/sequence.hpp>

/**
 * @brief Generates a comma-separated sequence using a user macro.
 *
 * Expands to a comma-separated list of elements produced by
 * the given @p macro.
 *
 * The @p macro is expected to have the following signature:
 * @code
 * macro(n, ...)
 * @endcode
 * where:
 * - @p n is the 1-based index of the element in the sequence;
 * - @p ... is arbitrary user data passed through from the `NX_REPEAT` call.
 *
 * @par Example
 * @code
 * #define MY_MACRO(n, name) name##_##n
 * int NX_REPEAT(3, MY_MACRO, var);
 * // Expands to:
 * // int var_1, var_2, var_3;
 * @endcode
 *
 * @param count Number of elements in the sequence.
 * @param macro User-defined macro used to generate each element.
 * @param ...   User data forwarded to @p macro together with the index.
 */
#define NX_REPEAT(count, macro, ...) \
    _nx_make_sequence(count, macro, __VA_ARGS__)


/**
 * @brief Generates a sequence of elements using a user macro, without separators.
 *
 * `NX_REPEAT_NO_COMMA` behaves similarly to `NX_REPEAT`, but does **not**
 * insert commas (or any other separators) between generated elements.
 *
 * The @p macro must have the following signature:
 * @code
 * macro(n, ...)
 * @endcode
 * where:
 * - @p n is the 1-based index of the element in the sequence;
 * - @p ... is arbitrary user data passed through from the `NX_REPEAT_NO_COMMA` call.
 *
 * @par Example
 * @code
 * #define MY_MACRO(n, token) NX_TO_STRING(token##n)
 *
 * const char* s = NX_REPEAT_NO_COMMA(3, MY_MACRO, letter);
 * // Expands to:
 * // const char* s = "letter1" "letter2" "letter3";
 * // (three repetitions concatenated, no comma or separator)
 * @endcode
 *
 * @param count Number of elements in the sequence.
 * @param macro User-defined macro used to generate each element.
 * @param ...   User data forwarded to @p macro together with the index.
 */
#define NX_REPEAT_NO_COMMA(count, macro, ...) \
    _nx_make_sequence_no_decorator(count, macro, __VA_ARGS__)


#endif //REPEAT_HPP
