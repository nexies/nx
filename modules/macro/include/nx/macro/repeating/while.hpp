//
// Created by nexie on 21.12.2025.
//

#ifndef NX_MACRO_WHILE_HPP
#define NX_MACRO_WHILE_HPP

#include <nx/macro.hpp>

#include <nx/macro/logic/if.hpp>
#include <nx/macro/detail/while_limit.hpp>
#include <nx/macro/numeric/inc_dec.hpp>

#define _nx_while(c, o, r, ...) \
    _nx_while_0(c, o, r, __VA_ARGS__)

#define _nx_while_d(d, c, o, r, ...) \
    _nx_while_##d(c, o, r, __VA_ARGS__)

/**
 * @brief Represents a preprocessor-level looping construct.
 *
 * `NX_WHILE` repeatedly applies the given @p operation to the current loop state
 * while the @p condition predicate evaluates to true. The result of the loop
 * is produced through the @p result macro.
 *
 * This macro is evaluated entirely at preprocessing time and does not
 * generate runtime code.
 *
 * All three functional parameters (@p condition, @p operation and @p result)
 * are themselves preprocessor macros and must follow a specific calling
 * convention:
 *
 * - @p condition
 *   - Signature: `CONDITION(d, state...)`
 *   - @p d is the current recursion/iteration depth.
 *   - @p state... is the current loop state (one or more preprocessor tokens).
 *   - Must expand to either `0` or `1` depending on whether the loop should
 *     continue (`1`) or terminate (`0`) for the given state.
 *
 * - @p operation
 *   - Signature: `OPERATION(d, state...)`
 *   - @p d is the current recursion/iteration depth.
 *   - @p state... is the current loop state.
 *   - Must expand to the next loop state, i.e. the state that will be passed
 *     to the next iteration. For example, this macro may increment a numeric
 *     component using NX_INC or combine/transform tuple-like arguments.
 *
 * - @p result
 *   - Signature: `RESULT(d, state...)`
 *   - @p d is the current recursion/iteration depth at the moment the loop
 *     terminates.
 *   - @p state... is the final loop state after the last application of
 *     @p operation.
 *   - Must expand to the final value of the loop, i.e. extract or compute the
 *     “user-visible” result from the final internal state.
 *
 * The exact structure of the loop state (@p state...) is user-defined, but
 * must be handled consistently by all three macros.
 *
 * @par Example
 * @code
    #define SUM_MANY_COND(d, res, ...) \
        NX_BOOL(NX_ARGS_COUNT(__VA_ARGS__))

    #define SUM_MANY_OP(d, res, cur, ...) \
        NX_NUMERIC_SUM_D(d, cur, res) NX_APPEND_ARGS(__VA_ARGS__)

    #define SUM_MANY_RES(d, res, ...) \
        res

    #define SUM_MANY(...) \
        NX_WHILE \
        ( \
            SUM_MANY_COND, \
            SUM_MANY_OP, \
            SUM_MANY_RES, \
            0, __VA_ARGS__ \
        )

    SUM_MANY(2, 3, 4, 5) // Expands to: 14
    SUM_MANY() // Expands to: 0
 * @endcode
 *
 * @param condition Predicate macro used to determine whether to continue looping.
 * @param operation Macro applied on each iteration to produce the next state.
 * @param result Macro used to extract/produce the final loop value.
 * @param ... Additional arguments representing the loop state.
 *
 * @see NX_BOOL
 * @see NX_ARGS_COUNT
 * @see NX_APPEND_ARGS
 * @see NX_NUMERIC_SUM_D
 * @see NX_WHILE_D
 */
#define NX_WHILE(condition, operation, result, ...) \
    _nx_while(condition, operation, result, __VA_ARGS__)



/**
 * @brief Represents a preprocessor-level looping construct.
 *
 * `NX_WHILE_D` repeatedly applies the given @p operation to the current loop state
 * while the @p condition predicate evaluates to true. The result of the loop
 * is produced through the @p result macro.
 *
 * This macro is evaluated entirely at preprocessing time and does not
 * generate runtime code.
 *
 * This macro is similar to `NX_WHILE`, but it also takes current recursion/iteration
 * depth as a parameter, allowing it to be executed inside another `NX_WHILE` loop
 *
 * All three functional parameters (@p condition, @p operation and @p result)
 * are themselves preprocessor macros and must follow a specific calling
 * convention:
 *
 * - @p condition
 *   - Signature: `CONDITION(d, state...)`
 *   - @p d is the current recursion/iteration depth.
 *   - @p state... is the current loop state (one or more preprocessor tokens).
 *   - Must expand to either `0` or `1` depending on whether the loop should
 *     continue (`1`) or terminate (`0`) for the given state.
 *
 * - @p operation
 *   - Signature: `OPERATION(d, state...)`
 *   - @p d is the current recursion/iteration depth.
 *   - @p state... is the current loop state.
 *   - Must expand to the next loop state, i.e. the state that will be passed
 *     to the next iteration. For example, this macro may increment a numeric
 *     component using NX_INC or combine/transform tuple-like arguments.
 *
 * - @p result
 *   - Signature: `RESULT(d, state...)`
 *   - @p d is the current recursion/iteration depth at the moment the loop
 *     terminates.
 *   - @p state... is the final loop state after the last application of
 *     @p operation.
 *   - Must expand to the final value of the loop, i.e. extract or compute the
 *     “user-visible” result from the final internal state.
 *
 * The exact structure of the loop state (@p state...) is user-defined, but
 * must be handled consistently by all three macros.
 *
 * @par Example
 * @code
    #define SUM_MANY_COND(d, res, ...) \
        NX_BOOL(NX_ARGS_COUNT(__VA_ARGS__))

    #define SUM_MANY_OP(d, res, cur, ...) \
        NX_NUMERIC_SUM_D(d, cur, res) NX_APPEND_ARGS(__VA_ARGS__)

    #define SUM_MANY_RES(d, res, ...) \
        res

    #define SUM_MANY_D(d, ...) \
        NX_WHILE_D(d, \
            SUM_MANY_COND, \
            SUM_MANY_OP, \
            SUM_MANY_RES, \
            0, __VA_ARGS__ \
        )

    SUM_MANY_D(0, 2, 3, 4, 5) // Expands to: 14
    SUM_MANY_D(0) // Expands to: 0
 * @endcode
 *
 * @param condition Predicate macro used to determine whether to continue looping.
 * @param operation Macro applied on each iteration to produce the next state.
 * @param result Macro used to extract/produce the final loop value.
 * @param ... Additional arguments representing the loop state.
 *
 * @see NX_BOOL
 * @see NX_ARGS_COUNT
 * @see NX_APPEND_ARGS
 * @see NX_NUMERIC_SUM_D
 * @see NX_WHILE_D
 */
#define NX_WHILE_D(d, condition, operation, result, ...) \
    _nx_while_d(d, condition, operation, result, __VA_ARGS__)

#endif //WHILE_HPP
