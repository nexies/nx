//
// logger_defs — convenience logging macros for the core2 module.
//

#pragma once

#include <nx/logging/macros.hpp>
#include <nx/logging/registry.hpp>

#ifndef NX_TRACE_SIGNALS
#    define NX_TRACE_SIGNALS 1
#endif

#ifndef NX_DEVEL_LOGGING
#    define NX_DEVEL_LOGGING 1
#endif

#define nxTrace(...)    NX_LOG_LOGGER_TRACE(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxDebug(...)    NX_LOG_LOGGER_DEBUG(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxInfo(...)     NX_LOG_LOGGER_INFO(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxWarning(...)  NX_LOG_LOGGER_WARN(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxError(...)    NX_LOG_LOGGER_ERROR(::nx::logging::get_default_logger(), __VA_ARGS__)
#define nxCritical(...) NX_LOG_LOGGER_CRITICAL(::nx::logging::get_default_logger(), __VA_ARGS__)
