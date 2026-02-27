//
// Created by nexie on 27.02.2026.
//

#ifndef NX_LOGGER_DEFS_HPP
#define NX_LOGGER_DEFS_HPP

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
// #define SPDLOG_FUNCTION static_cast<const char *>(__PRETTY_FUNCTION__)
#include <spdlog/spdlog.h>

#define NX_TRACE_SIGNALS 1
#define NX_TRACE_SIGNALS_LOGGER_NAME "signals"

#define nxTrace(...)    SPDLOG_LOGGER_TRACE(::spdlog::default_logger(), __VA_ARGS__)
#define nxDebug(...)    SPDLOG_LOGGER_DEBUG(::spdlog::default_logger(), __VA_ARGS__)
#define nxInfo(...)     SPDLOG_LOGGER_INFO(::spdlog::default_logger(), __VA_ARGS__)
#define nxWarning(...)  SPDLOG_LOGGER_WARN(::spdlog::default_logger(), __VA_ARGS__)
#define nxError(...)    SPDLOG_LOGGER_ERROR(::spdlog::default_logger(), __VA_ARGS__)
#define nxCritical(...) SPDLOG_LOGGER_CRITICAL(::spdlog::default_logger(), __VA_ARGS__)

#endif //NX_LOGGER_DEFS_HPP