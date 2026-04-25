//
// signal_defs — NX_SIGNAL and NX_EMIT macros.
//
// NX_SIGNAL(name, [type1, type2, ...])
//   Generates a public void method that, when called, dispatches to all
//   connected slots via nx::core::emit().
//
// NX_EMIT(name, [arg1, arg2, ...])
//   Calls the signal method. Prefer this over direct calls so that signal
//   tracing (NX_TRACE_SIGNALS) can be inserted transparently.
//

#pragma once

#include <nx/macro/args/count.hpp>
#include <nx/macro/repeating/repeat.hpp>
#include <nx/macro/util/choose.hpp>

// ──────────────────────────────────────────────────────────────────────────────
// Internal helpers: generate typed parameter lists and forwarding param lists
//
//   __NX_SIG_ARG_DECL(n, T1, T2)  →  T1 _a1, T2 _a2
//   __NX_SIG_ARG_FWD(n, T1, T2)   →  _a1, _a2
// ──────────────────────────────────────────────────────────────────────────────

// NOLINTBEGIN(readability-identifier-naming)

#define __NX_SIG_ARG_DECL_ITEM(n, ...) \
    NX_CHOOSE(n,, __VA_ARGS__) NX_CONCAT(_a, n)

#define __NX_SIG_ARG_DECL(...) \
    NX_REPEAT(NX_ARGS_COUNT(__VA_ARGS__), __NX_SIG_ARG_DECL_ITEM, __VA_ARGS__)

#define __NX_SIG_ARG_FWD_ITEM(n, ...) \
    NX_CONCAT(_a, n)

#define __NX_SIG_ARG_FWD(...) \
    NX_REPEAT(NX_ARGS_COUNT(__VA_ARGS__), __NX_SIG_ARG_FWD_ITEM, __VA_ARGS__)

// ──────────────────────────────────────────────────────────────────────────────
// __NX_SIGNAL_0  — no-argument signal
// __NX_SIGNAL_N  — signal with argument types
// ──────────────────────────────────────────────────────────────────────────────

// #define __NX_SIGNAL_0(name)                                                     \
//     void name()                                                                  \
//     {                                                                            \
//         using _Self = std::remove_cv_t<std::remove_reference_t<decltype(*this)>>;\
//         ::nx::core::emit(this,                                                   \
//             static_cast<void(_Self::*)()>(&_Self::name));                        \
//     }
//
// #define __NX_SIGNAL_N(name, ...)                                                 \
//     void name(__NX_SIG_ARG_DECL(__VA_ARGS__))                                   \
//     {                                                                            \
//         using _Self = std::remove_cv_t<std::remove_reference_t<decltype(*this)>>;\
//         ::nx::core::emit(this,                                                   \
//             static_cast<void(_Self::*)(__VA_ARGS__)>(&_Self::name),              \
//             __NX_SIG_ARG_FWD(__VA_ARGS__));                                      \
//     }

// Dispatch on whether __VA_ARGS__ is empty
#define __NX_SIGNAL_DISPATCH(name, ...)                                          \
    NX_CONCAT(__NX_SIGNAL_, __NX_PP_ISEMPTY(__VA_ARGS__))(name, __VA_ARGS__)

// __NX_PP_ISEMPTY returns 1 when args are empty, 0 when args are present.
// So ISEMPTY=0 (has args) → __NX_SIGNAL_0 handles the argument case.
//    ISEMPTY=1 (no args)  → __NX_SIGNAL_1 handles the no-argument case.

// ISEMPTY=0: signal WITH arguments
#ifdef __NX_SIGNAL_0
#undef __NX_SIGNAL_0
#endif

#define __NX_SIGNAL_0(name, ...)                                                 \
    void name(__NX_SIG_ARG_DECL(__VA_ARGS__))                                   \
    {                                                                            \
        using SelfType = std::remove_cv_t<std::remove_reference_t<decltype(*this)>>;\
        ::nx::core::emit(this,                                                   \
            static_cast<void(SelfType::*)(__VA_ARGS__)>(&SelfType::name),              \
            __NX_SIG_ARG_FWD(__VA_ARGS__));                                      \
    }


#ifdef __NX_SIGNAL_1
#undef __NX_SIGNAL_1
#endif

// ISEMPTY=1: signal with NO arguments
#define __NX_SIGNAL_1(name, ...)                                                 \
    void name()                                                                  \
    {                                                                            \
        using SelfType = std::remove_cv_t<std::remove_reference_t<decltype(*this)>>;\
        ::nx::core::emit(this,                                                   \
            static_cast<void(SelfType::*)()>(&SelfType::name));                        \
    }

// ──────────────────────────────────────────────────────────────────────────────
// Public API
// ──────────────────────────────────────────────────────────────────────────────

/// NX_SIGNAL(name)            — signal with no arguments
/// NX_SIGNAL(name, T1, T2)    — signal forwarding T1 and T2 to connected slots
#define NX_SIGNAL(name, ...)                                                     \
    NX_CONCAT(__NX_SIGNAL_, __NX_PP_ISEMPTY(__VA_ARGS__))(name, __VA_ARGS__)

// Signal tracing (opt-in: define NX_TRACE_SIGNALS=1 before including)
#ifndef NX_TRACE_SIGNALS
#   define NX_TRACE_SIGNALS 0
#endif

#if NX_TRACE_SIGNALS
#   include <nx/logging/macros.hpp>
#   include <nx/logging/registry.hpp>
#   ifndef NX_TRACE_SIGNALS_LOGGER_NAME
#       define NX_TRACE_SIGNALS_LOGGER_NAME "signals"
#   endif
#   define __NX_EMIT_TRACE(name, ...)                                            \
        NX_LOG_LOGGER_TRACE(::nx::logging::get(NX_TRACE_SIGNALS_LOGGER_NAME),   \
                            "emit " #name "(" #__VA_ARGS__ ")")
#else
#   define __NX_EMIT_TRACE(name, ...) (void)0
#endif

/// NX_EMIT(signal_name [, arg1, arg2, ...])
/// Calls the signal method and optionally logs the emission.
#define NX_EMIT(name, ...)      \
    do {    \
    __NX_EMIT_TRACE(name, __VA_ARGS__); \
    name(__VA_ARGS__);   \
    } while (0);


// NOLINTEND(readability-identifier-naming)
