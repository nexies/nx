//
// Created by nexie on 27.02.2026.
//

#ifndef NX_LOGGER_DEFS_HPP
#define NX_LOGGER_DEFS_HPP

#include <nx/logging/macros.hpp>
#include <nx/logging/registry.hpp>

#ifndef NX_TRACE_SIGNALS
#    define NX_TRACE_SIGNALS 1
#endif
#define NX_TRACE_SIGNALS_LOGGER_NAME "signals"

#ifndef NX_DEVEL_LOGGING
#    define NX_DEVEL_LOGGING 1
#endif
#define NX_DEVEL_LOGGER_NAME "devel"

#define nxTrace(...)    NX_LOG_LOGGER_TRACE(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxDebug(...)    NX_LOG_LOGGER_DEBUG(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxInfo(...)     NX_LOG_LOGGER_INFO(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxWarning(...)  NX_LOG_LOGGER_WARN(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxError(...)    NX_LOG_LOGGER_ERROR(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxCritical(...) NX_LOG_LOGGER_CRITICAL(::nx::logging::get_default_logger(), __VA_ARGS__)

#if NX_DEVEL_LOGGING
#    define nxDevTrace(...)    NX_LOG_LOGGER_TRACE(::nx::logging::get(NX_DEVEL_LOGGER_NAME), __VA_ARGS__)
#    define nxDevDebug(...)    NX_LOG_LOGGER_DEBUG(::nx::logging::get(NX_DEVEL_LOGGER_NAME), __VA_ARGS__)
#    define nxDevInfo(...)     NX_LOG_LOGGER_INFO(::nx::logging::get(NX_DEVEL_LOGGER_NAME), __VA_ARGS__)
#    define nxDevWarning(...)  NX_LOG_LOGGER_WARN(::nx::logging::get(NX_DEVEL_LOGGER_NAME), __VA_ARGS__)
#    define nxDevError(...)    NX_LOG_LOGGER_ERROR(::nx::logging::get(NX_DEVEL_LOGGER_NAME), __VA_ARGS__)
#    define nxDevCritical(...) NX_LOG_LOGGER_CRITICAL(::nx::logging::get(NX_DEVEL_LOGGER_NAME), __VA_ARGS__)
#else
#    define nxDevTrace(...)
#    define nxDevDebug(...)
#    define nxDevInfo(...)
#    define nxDevWarning(...)
#    define nxDevError(...)
#    define nxDevCritical(...)
#endif

#endif // NX_LOGGER_DEFS_HPP
