//
// Created by nexie on 21.11.2025.
//

#ifndef SIGNAL_DEFS_HPP
#define SIGNAL_DEFS_HPP

///
///  vision:
///
/// 1)
///     declaration:
///         void signalName(arg1, arg2, ...)
///         {   NX_SIGNAL_DEF(arg1, arg2, ...); }
///     usage:
///         signalName(1, 2, 3);
///
///  2)
///     declaration:
///         NX_SIGNAL(signalName, args1, args2, ...);
///     usage:
///         nx::Object::emit(signalName, args1, args2m ...);
///
///  3) would be cool:
///     NX_SIGNAL(signalName, arg_type1, arg_type2, ...);
///
///     signalName(1, 2, 3);
///
///
///
///     NX_SIGNAL(signalName, arg_type1, arg_type2, arg_type3) -->
///
///     void signalName (arg_type1 _arg1, arg_type2 _arg2, arg_type3 _arg3)


#include <nx/macro/repeat.hpp>
#include <nx/macro/util/choose.hpp>
#include <nx/macro/util/arg_count.hpp>

#include <nx/core.hpp>
#include <nx/core/Overload.h>

#define __NX_SIGNAL_ARGUMENT_MAKER_MACRO(n, prefix, ...) \
    NX_CHOOSE(n,,__VA_ARGS__) NX_CONCAT(prefix, n)

#define __NX_SIGNAL_MAKE_ARGUMENTS(...) \
    NX_REPEAT(NX_ARGS_COUNT(__VA_ARGS__), __NX_SIGNAL_ARGUMENT_MAKER_MACRO, _arg, __VA_ARGS__)

#define __NX_SIGNAL_PARAMS_MAKER_MACRO(n, prefix, ...) \
    NX_CONCAT(prefix, n)

#define __NX_SIGNAL_MAKE_PARAMS(...) \
    NX_REPEAT(NX_ARGS_COUNT(__VA_ARGS__), __NX_SIGNAL_PARAMS_MAKER_MACRO, _arg, __VA_ARGS__)

#define __NX_SIGNAL_HELPER_0(signalName, ...) \
void signalName (__NX_SIGNAL_MAKE_ARGUMENTS(__VA_ARGS__)) \
    { \
        using This = std::remove_cv_t<std::remove_reference_t<decltype(*this)>>; \
        ::nx::emit(this, ::nx::overload<__VA_ARGS__>(&This::signalName), __NX_SIGNAL_MAKE_PARAMS(__VA_ARGS__)); \
    } \

#define __NX_SIGNAL_HELPER_1(signalName, ...) \
    void signalName () \
    { \
        using This = std::remove_cv_t<std::remove_reference_t<decltype(*this)>>; \
        ::nx::emit(this, ::nx::overload<>(&This::signalName)); \
    } \

#define __NX_SIGNAL_HELPER(signalName, ...) \
    NX_CONCAT(__NX_SIGNAL_HELPER_, __NX_PP_ISEMPTY(__VA_ARGS__))(signalName, __VA_ARGS__)

#define NX_SIGNAL(signalName, ...) \
    __NX_SIGNAL_HELPER(signalName, __VA_ARGS__)

#ifndef NX_TRACE_SIGNALS
#define NX_TRACE_SIGNALS 0
#define NX_TRACE_SIGNALS_LOGGER_NAME "signals"
#endif

#if NX_TRACE_SIGNALS
#   define __NX_SIGNAL_LOGGER_CALL_0(signalName, ...) \
        SPDLOG_LOGGER_TRACE(::spdlog::get(NX_TRACE_SIGNALS_LOGGER_NAME), "emmiting " #signalName "()")

#   define __NX_SIGNAL_LOGGER_CALL_1(signalName, ...) \
        SPDLOG_LOGGER_TRACE(::spdlog::get(NX_TRACE_SIGNALS_LOGGER_NAME), "emmiting " #signalName "(" #__VA_ARGS__")")

#   define __NX_SIGNAL_LOGGER_CALL(signalName, ...) \
        NX_CONCAT(__NX_SIGNAL_LOGGER_CALL_, NX_HAS_ARGS(__VA_ARGS__))(signalName, __VA_ARGS__)
#else
#   define __NX_SIGNAL_LOGGER_CALL(...) NX_CONSUME(__VA_ARGS__)
#endif

#ifndef __NX_BETTER_SIGNALS
#define NX_EMIT(signalName, ...)  \
    __NX_SIGNAL_LOGGER_CALL(signalName, __VA_ARGS__); \
    signalName( __VA_ARGS__ );
#else
#define NX_EMIT(signalName, ...) \
    __NX_SIGNAL_LOGGER_CALL(signalName, __VA_ARGS__); \
    signalName.emit( __VA_ARGS__ )
#endif

#define NX_MAKE_EMIT(signalRef, objectPtr, ...) \
    NX_EMIT((objectPtr->*signalRef), __VA_ARGS__)

#endif //SIGNAL_DEFS_HPP
