//
// Created by nexie on 27.02.2026.
//

#ifndef NX_LOGGER_DEFS_HPP
#define NX_LOGGER_DEFS_HPP

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
// #define SPDLOG_FUNCTION static_cast<const char *>(__PRETTY_FUNCTION__)
#include <spdlog/spdlog.h>

#ifndef NX_TRACE_SIGNALS
#define NX_TRACE_SIGNALS 1
#endif
#define NX_TRACE_SIGNALS_LOGGER_NAME "signals"

#ifndef NX_DEVEL_LOGGING
#define NX_DEVEL_LOGGING 1
#endif
#define NX_DEVEL_LOGGER_NAME "devel"

#define nxTrace(...)    SPDLOG_LOGGER_TRACE(::spdlog::default_logger(), __VA_ARGS__)
#define nxDebug(...)    SPDLOG_LOGGER_DEBUG(::spdlog::default_logger(), __VA_ARGS__)
#define nxInfo(...)     SPDLOG_LOGGER_INFO(::spdlog::default_logger(), __VA_ARGS__)
#define nxWarning(...)  SPDLOG_LOGGER_WARN(::spdlog::default_logger(), __VA_ARGS__)
#define nxError(...)    SPDLOG_LOGGER_ERROR(::spdlog::default_logger(), __VA_ARGS__)
#define nxCritical(...) SPDLOG_LOGGER_CRITICAL(::spdlog::default_logger(), __VA_ARGS__)


#if NX_DEVEL_LOGGING
#define nxDevTrace(...)    SPDLOG_LOGGER_TRACE(::spdlog::get(NX_DEVEL_LOGGER_NAME), __VA_ARGS__)
#define nxDevDebug(...)    SPDLOG_LOGGER_DEBUG(::spdlog::get(NX_DEVEL_LOGGER_NAME), __VA_ARGS__)
#define nxDevInfo(...)     SPDLOG_LOGGER_INFO(::spdlog::get(NX_DEVEL_LOGGER_NAME), __VA_ARGS__)
#define nxDevWarning(...)  SPDLOG_LOGGER_WARN(::spdlog::get(NX_DEVEL_LOGGER_NAME), __VA_ARGS__)
#define nxDevError(...)    SPDLOG_LOGGER_ERROR(::spdlog::get(NX_DEVEL_LOGGER_NAME), __VA_ARGS__)
#define nxDevCritical(...) SPDLOG_LOGGER_CRITICAL(::spdlog::get(NX_DEVEL_LOGGER_NAME), __VA_ARGS__)
#else
#define nxDevTrace(...)
#define nxDevDebug(...)
#define nxDevInfo(...)
#define nxDevWarning(...)
#define nxDevError(...)
#define nxDevCritical(...)
#endif

#endif //NX_LOGGER_DEFS_HPP