//
// Created by green on 3/21/2026.
//

#ifndef NX_ASIO_BACKEND_TYPES_HPP
#define NX_ASIO_BACKEND_TYPES_HPP

#include <nx/core/detail/units.hpp>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <chrono>

namespace nx::asio {

# ifdef _WIN32
    using NativeHandle = void *;
    constexpr static NativeHandle g_nullHandle = NULL;
#else
    using NativeHandle = int;
    constexpr static NativeHandle g_nullHandle = 0;
# endif

    enum class IOInterest : std::uint32_t {
        None    = 0x00,
        Read    = 0x01,
        Write   = 0x02
    };

    inline IOInterest
    operator & (IOInterest a, IOInterest b) {
        using U = std::underlying_type_t<IOInterest>;
        return static_cast<IOInterest>(static_cast<U>(a) & static_cast<U>(b));
    }

    inline IOInterest
    operator | (IOInterest a, IOInterest b) {
        using U = std::underlying_type_t<IOInterest>;
        return static_cast<IOInterest>(static_cast<U>(a) | static_cast<U>(b));
    }

    inline IOInterest
    operator ^ (IOInterest a, IOInterest b) {
        using U = std::underlying_type_t<IOInterest>;
        return static_cast<IOInterest>(static_cast<U>(a) ^ static_cast<U>(b));
    }

    enum class IOEvent : std::uint32_t {
        None    = 0x00,
        Read    = 0x01,
        Write   = 0x02,
        Error   = 0x03,
        Hangup  = 0x04,
        Wakeup  = 0x05,
    };

    inline IOEvent
    operator & (IOEvent a, IOEvent b) {
        using U = std::underlying_type_t<IOEvent>;
        return static_cast<IOEvent>(static_cast<U>(a) & static_cast<U>(b));
    }

    inline IOEvent
    operator | (IOEvent a, IOEvent b) {
        using U = std::underlying_type_t<IOEvent>;
        return static_cast<IOEvent>(static_cast<U>(a) | static_cast<U>(b));
    }

    inline IOEvent
    operator ^ (IOEvent a, IOEvent b) {
        using U = std::underlying_type_t<IOEvent>;
        return static_cast<IOEvent>(static_cast<U>(a) ^ static_cast<U>(b));
    }


    struct BackendEvent {
        void * token = nullptr;
        NativeHandle identity { 0 };
        IOEvent events = IOEvent::None;
        uint64_t u64 { 0 };
        uint32_t u32 { 0 };
    };

    using Clock = std::chrono::system_clock;
    using Duration = Clock::duration;
    using TimePoint = Clock::time_point;

    using TimerId = ::nx::TimerId;

}

#endif //NX_ASIO_BACKEND_TYPES_HPP