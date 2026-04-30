//
// Compile-time level stripping + logger macros (replaces spdlog macros in nx).
//

#ifndef NX_LOGGING_MACROS_HPP
#define NX_LOGGING_MACROS_HPP

#include <nx/common/types/source_location.hpp>
#include <nx/logging/levels.hpp>
#include <nx/logging/logger.hpp>

// Minimum level compiled in: 0=trace … 6=off (matches nx::logging::level).
#ifndef NX_LOG_COMPILE_LEVEL
#    define NX_LOG_COMPILE_LEVEL 0
#endif

#define NX_LOG_LEVEL_TRACE 0
#define NX_LOG_LEVEL_DEBUG 1
#define NX_LOG_LEVEL_INFO 2
#define NX_LOG_LEVEL_WARN 3
#define NX_LOG_LEVEL_ERR 4
#define NX_LOG_LEVEL_CRITICAL 5
#define NX_LOG_LEVEL_OFF 6

#if NX_LOG_COMPILE_LEVEL <= NX_LOG_LEVEL_TRACE
#    define NX_LOG_LOGGER_TRACE(lg, ...)                                                                               \
        do {                                                                                                           \
            auto _nx_log_lg = (lg);                                                                                    \
            if (_nx_log_lg && _nx_log_lg->should_log(::nx::logging::level::trace))                                     \
                _nx_log_lg->log(::nx::logging::level::trace, ::nx::source_location::current(), __VA_ARGS__);           \
        } while (0)
#else
#    define NX_LOG_LOGGER_TRACE(lg, ...) (void)0
#endif

#if NX_LOG_COMPILE_LEVEL <= NX_LOG_LEVEL_DEBUG
#    define NX_LOG_LOGGER_DEBUG(lg, ...)                                                                               \
        do {                                                                                                           \
            auto _nx_log_lg = (lg);                                                                                    \
            if (_nx_log_lg && _nx_log_lg->should_log(::nx::logging::level::debug))                                     \
                _nx_log_lg->log(::nx::logging::level::debug, ::nx::source_location::current(), __VA_ARGS__);            \
        } while (0)
#else
#    define NX_LOG_LOGGER_DEBUG(lg, ...) (void)0
#endif

#if NX_LOG_COMPILE_LEVEL <= NX_LOG_LEVEL_INFO
#    define NX_LOG_LOGGER_INFO(lg, ...)                                                                                \
        do {                                                                                                           \
            auto _nx_log_lg = (lg);                                                                                    \
            if (_nx_log_lg && _nx_log_lg->should_log(::nx::logging::level::info))                                      \
                _nx_log_lg->log(::nx::logging::level::info, ::nx::source_location::current(), __VA_ARGS__);            \
        } while (0)
#else
#    define NX_LOG_LOGGER_INFO(lg, ...) (void)0
#endif

#if NX_LOG_COMPILE_LEVEL <= NX_LOG_LEVEL_WARN
#    define NX_LOG_LOGGER_WARN(lg, ...)                                                                                \
        do {                                                                                                           \
            auto _nx_log_lg = (lg);                                                                                    \
            if (_nx_log_lg && _nx_log_lg->should_log(::nx::logging::level::warn))                                      \
                _nx_log_lg->log(::nx::logging::level::warn, ::nx::source_location::current(), __VA_ARGS__);           \
        } while (0)
#else
#    define NX_LOG_LOGGER_WARN(lg, ...) (void)0
#endif

#if NX_LOG_COMPILE_LEVEL <= NX_LOG_LEVEL_ERR
#    define NX_LOG_LOGGER_ERROR(lg, ...)                                                                               \
        do {                                                                                                           \
            auto _nx_log_lg = (lg);                                                                                    \
            if (_nx_log_lg && _nx_log_lg->should_log(::nx::logging::level::err))                                       \
                _nx_log_lg->log(::nx::logging::level::err, ::nx::source_location::current(), __VA_ARGS__);             \
        } while (0)
#else
#    define NX_LOG_LOGGER_ERROR(lg, ...) (void)0
#endif

#if NX_LOG_COMPILE_LEVEL <= NX_LOG_LEVEL_CRITICAL
#    define NX_LOG_LOGGER_CRITICAL(lg, ...)                                                                            \
        do {                                                                                                           \
            auto _nx_log_lg = (lg);                                                                                    \
            if (_nx_log_lg && _nx_log_lg->should_log(::nx::logging::level::critical))                                \
                _nx_log_lg->log(::nx::logging::level::critical, ::nx::source_location::current(), __VA_ARGS__);       \
        } while (0)
#else
#    define NX_LOG_LOGGER_CRITICAL(lg, ...) (void)0
#endif


#define nxTrace(...)    NX_LOG_LOGGER_TRACE(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxDebug(...)    NX_LOG_LOGGER_DEBUG(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxInfo(...)     NX_LOG_LOGGER_INFO(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxWarning(...)  NX_LOG_LOGGER_WARN(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxError(...)    NX_LOG_LOGGER_ERROR(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxCritical(...) NX_LOG_LOGGER_CRITICAL(::nx::logging::get_default_logger(), __VA_ARGS__)

#define nx_trace(...)    NX_LOG_LOGGER_TRACE(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nx_debug(...)    NX_LOG_LOGGER_DEBUG(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nx_info(...)     NX_LOG_LOGGER_INFO(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nx_warning(...)  NX_LOG_LOGGER_WARN(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nx_error(...)    NX_LOG_LOGGER_ERROR(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nx_critical(...) NX_LOG_LOGGER_CRITICAL(::nx::logging::get_default_logger(), __VA_ARGS__)



#endif // NX_LOGGING_MACROS_HPP
