//
// Created by nexie on 27.02.2026.
//

#ifndef NX_CORE_DETAIL_UNITS_HPP
#define NX_CORE_DETAIL_UNITS_HPP

#include <chrono>
#include <thread>

namespace nx
{
    using Clock         = std::chrono::high_resolution_clock;
    using Duration      = Clock::duration;
    using TimePoint     = Clock::time_point;

    using Days          = std::chrono::days;
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

#endif //NX_CORE_DETAIL_UNITS_HPP