//
// Created by nexie on 21.12.2025.
//

#ifndef NX_MACRO_ITERATE_HPP
#define NX_MACRO_ITERATE_HPP

#include <nx/macro.hpp>
#include <nx/macro/util/choose.hpp>
#include <nx/macro/util/arg_count.hpp>
#include <nx/macro/repeating/sequence.hpp>

#define _nx_sequence_iterator(n, macro, ...) \
    macro(n, _nx_choose(n, , __VA_ARGS__))

#define _nx_make_iterate(macro, ...) \
    _nx_make_sequence_no_decorator(_nx_args_count(__VA_ARGS__), _nx_sequence_iterator, macro, __VA_ARGS__)

#define _nx_make_decorated_iterate(macro, decorator, ...) \
    _nx_make_decorated_sequence(_nx_args_count(__VA_ARGS__), decorator, _nx_sequence_iterator, macro, __VA_ARGS__)


/**
 * @brief Iterates over a variadic list of parameters using a user-defined iterator macro.
 *
 * `NX_ITERATE` invokes the provided @p iterator macro once per element in the
 * parameter list, passing the element and its positional index.
 *
 * The @p iterator macro must have the following signature:
 * @code
 * iterator(n, element)
 * @endcode
 * where:
 * - @p n       — 1-based positional index of the element,
 * - @p element — the element from the list.
 *
 * @par Example
 * @code
 * #define MY_ITERATOR(n, type) type value##n;
 *
 * NX_ITERATE(MY_ITERATOR, int, float, double, bool)
 * // Expands to:
 * // int value1; float value2; double value3; bool value4;
 * @endcode
 *
 * @param iterator User-defined iterator macro applied to each element.
 * @param ...      Variadic list of elements to iterate over.
 */
#define NX_ITERATE(iterator, ...) \
    _nx_make_iterate(iterator, __VA_ARGS__)

/**
 * @brief Iterates over a variadic list of elements and decorates each one.
 *
 * `NX_DECORATED_ITERATE` applies a user-defined @p iterator macro to each
 * element in the given variadic list and then passes the result through
 * a @p decorator macro.
 *
 * This allows formatting each element differently depending on its
 * position in the sequence (first, in-between, or last).
 *
 * The @p iterator macro must have the following signature:
 * @code
 * iterator(n, element)
 * @endcode
 * where:
 * - @p n       — 1-based index of the element in the list,
 * - @p element — the element itself.
 *
 * The @p decorator macro must have the following signature:
 * @code
 * decorator(c, data)
 * @endcode
 * where:
 * - @p c is a position code:
 *   - `0` — first element
 *   - `1` — middle element
 *   - `2` — last element
 * - @p data is the result of invoking @p iterator.
 *
 * @par Example
 * @code
 * #define DECORATOR_0(data) { data,
 * #define DECORATOR_1(data)   data,
 * #define DECORATOR_2(data)   data }
 * #define DECORATOR(c, data)  NX_CONCAT(DECORATOR_, c)(data)
 *
 * #define ITERATOR(n, elem) elem
 *
 * enum Enum {
 *     E1,
 *     E2,
 *     E3
 * };
 *
 * Enum vec[] = NX_DECORATED_ITERATE(ITERATOR, DECORATOR, E3, E2, E1);
 * // Expands to:
 * // { E3, E2, E1 }
 * @endcode
 *
 * @param iterator  User-defined macro applied to each element.
 * @param decorator User-defined macro decorating the iterator's output
 *                  depending on positional context.
 * @param ...       Elements to iterate over.
 */
#define NX_DECORATED_ITERATE(iterator, decorator, ...) \
    _nx_make_decorated_iterate(iterator, decorator, __VA_ARGS__)

#endif //ITERATE_HPP
