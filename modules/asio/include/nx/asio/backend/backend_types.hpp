//
// Created by green on 3/21/2026.
//

#ifndef NX_ASIO_BACKEND_TYPES_HPP
#define NX_ASIO_BACKEND_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <chrono>

namespace nx::asio {

# ifdef _WIN32
    using NativeHandle = void *;
#else
    using NativeHandle = int;
# endif

    enum class IOInterest : std::uint32_t {
        None    = 0x00,
        Read    = 0x01,
        Write   = 0x02
    };

    enum class IOEvent : std::uint32_t {
        None    = 0x00,
        Read    = 0x01,
        Write   = 0x02,
        Error   = 0x03,
        Hangup  = 0x04,
        Wakeup  = 0x05,
    };

    struct BackendEvent {
        void * token = nullptr;
        IOEvent events = IOEvent::None;
    };



}

#endif //NX_ASIO_BACKEND_TYPES_HPP