//
// Created by nexie on 12.11.2025.
//

#ifndef NXAPP_HPP
#define NXAPP_HPP

#include "core/Result.hpp"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#define nxTrace(...) SPDLOG_LOGGER_TRACE(::spdlog::default_logger(), __VA_ARGS__)
#define nxDebug(...) SPDLOG_LOGGER_DEBUG(::spdlog::default_logger(), __VA_ARGS__)
#define nxInfo(...)  SPDLOG_LOGGER_INFO(::spdlog::default_logger(), __VA_ARGS__)
#define nxWarning(...) SPDLOG_LOGGER_WARN(::spdlog::default_logger(), __VA_ARGS__)
#define nxError(...) SPDLOG_LOGGER_ERROR(::spdlog::default_logger(), __VA_ARGS__)
#define nxCritical(...) SPDLOG_LOGGER_CRITICAL(::spdlog::default_logger(), __VA_ARGS__)

#include <chrono>

namespace nx {
    using Clock = std::chrono::high_resolution_clock;
    using Duration = typename Clock::duration;
    using TimerPoint = typename Clock::time_point;

    // using Days    = std::chrono::days;
    using Hours   = std::chrono::hours;
    using Minutes = std::chrono::minutes;
    using Seconds = std::chrono::seconds;
    using Milliseconds = std::chrono::milliseconds;
    using Microseconds = std::chrono::microseconds;
    using Nanoseconds = std::chrono::nanoseconds;



}


#endif //NXAPP_HPP
