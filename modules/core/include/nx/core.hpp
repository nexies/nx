//
// Created by nexie on 11.11.2025.
//

#ifndef CORE_HPP
#define CORE_HPP
#define NX_TRACE_SIGNALS 1
#define NX_TRACE_SIGNALS_LOGGER_NAME "signal"
#include <nx/core/detail/signal_defs.hpp>
#include <nx/core/detail/property_defs.hpp>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
// #define SPDLOG_FUNCTION static_cast<const char *>(__PRETTY_FUNCTION__)
#include <spdlog/spdlog.h>

#define nxTrace(...)    SPDLOG_LOGGER_TRACE(::spdlog::default_logger(), __VA_ARGS__)
#define nxDebug(...)    SPDLOG_LOGGER_DEBUG(::spdlog::default_logger(), __VA_ARGS__)
#define nxInfo(...)     SPDLOG_LOGGER_INFO(::spdlog::default_logger(), __VA_ARGS__)
#define nxWarning(...)  SPDLOG_LOGGER_WARN(::spdlog::default_logger(), __VA_ARGS__)
#define nxError(...)    SPDLOG_LOGGER_ERROR(::spdlog::default_logger(), __VA_ARGS__)
#define nxCritical(...) SPDLOG_LOGGER_CRITICAL(::spdlog::default_logger(), __VA_ARGS__)

#include <chrono>
#include <thread>
#include <string>

// #include <nx/core/Object.hpp>
// #include <nx/core/Loop.hpp>
// #include <nx/core/Thread.hpp>
// #include <nx/core/Timer.hpp>


namespace nx
{
    namespace core
    {
        std::string version ();
    }

    using Clock         = std::chrono::high_resolution_clock;
    using Duration      = Clock::duration;
    using TimePoint     = Clock::time_point;

    // using Days    = std::chrono::days;
    using Hours         = std::chrono::hours;
    using Minutes       = std::chrono::minutes;
    using Seconds       = std::chrono::seconds;
    using Milliseconds  = std::chrono::milliseconds;
    using Microseconds  = std::chrono::microseconds;
    using Nanoseconds   = std::chrono::nanoseconds;

    using ThreadId          = size_t;
    using NativeThreadId    = std::thread::id;
    using ThreadHandle      = std::thread::native_handle_type;
    using TimerId           = size_t;

}


#endif //CORE_HPP
