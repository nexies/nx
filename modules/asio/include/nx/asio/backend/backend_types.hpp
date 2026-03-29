//
// Created by green on 3/21/2026.
//

#ifndef NX_ASIO_BACKEND_TYPES_HPP
#define NX_ASIO_BACKEND_TYPES_HPP

#include <nx/common/platform.hpp>

#include <cstdint>
#include <optional>
#include <chrono>

namespace nx::asio {

# ifdef NX_OS_WINDOWS
    typedef void* native_handle_t;
    constexpr static const native_handle_t g_null_handle = nullptr;
# else
    using native_handle_t = int;
    constexpr static native_handle_t g_null_handle = -1;
# endif

    enum class io_interest : std::uint32_t {
        none    = 0x00,
        read    = 0x01,
        write   = 0x02,
#if defined(NX_OS_APPLE)
        signal  = 0x04
#endif
    };

    inline io_interest
    operator & (io_interest a, io_interest b) {
        using U = std::underlying_type_t<io_interest>;
        return static_cast<io_interest>(static_cast<U>(a) & static_cast<U>(b));
    }

    inline io_interest
    operator | (io_interest a, io_interest b) {
        using U = std::underlying_type_t<io_interest>;
        return static_cast<io_interest>(static_cast<U>(a) | static_cast<U>(b));
    }

    inline io_interest
    operator ^ (io_interest a, io_interest b) {
        using U = std::underlying_type_t<io_interest>;
        return static_cast<io_interest>(static_cast<U>(a) ^ static_cast<U>(b));
    }

    enum class io_event : std::uint32_t {
        none    = 0x00,
        read    = 0x01,
        write   = 0x02,
        error   = 0x03,
        hangup  = 0x04,
        wakeup  = 0x05,
    };

    inline io_event
    operator & (io_event a, io_event b) {
        using U = std::underlying_type_t<io_event>;
        return static_cast<io_event>(static_cast<U>(a) & static_cast<U>(b));
    }

    inline io_event
    operator | (io_event a, io_event b) {
        using U = std::underlying_type_t<io_event>;
        return static_cast<io_event>(static_cast<U>(a) | static_cast<U>(b));
    }

    inline io_event
    operator ^ (io_event a, io_event b) {
        using U = std::underlying_type_t<io_event>;
        return static_cast<io_event>(static_cast<U>(a) ^ static_cast<U>(b));
    }


    struct backend_event {
        void * token = nullptr;
        native_handle_t identity { 0 };
        io_event events = io_event::none;
        uint64_t u64 { 0 };
        uint32_t u32 { 0 };

        union {
            struct {
                int signum;
                int reps;
            }signal;
            uint32_t bytes;
        } data {};
    };

    using clock = std::chrono::system_clock;
    using duration = clock::duration;
    using time_point = clock::time_point;

    // using timer_id = uint64_t;
    // static constexpr timer_id g_nullTimerId = std::numeric_limits<timer_id>::max();

    // static constexpr timer_id
    // nextTimerId(timer_id & tid) { while (tid++ == g_nullTimerId) {}; return tid; }

    struct timer_id
    {
    private:
        static constexpr size_t invalid_val = std::numeric_limits<size_t>::max();
        size_t val { 0 };
        constexpr void
        inc () { while (val == invalid_val) ++val; }
    public:
        static constexpr timer_id invalid() { return timer_id { std::numeric_limits<size_t>::max() }; }

        constexpr
        timer_id (size_t val = std::numeric_limits<size_t>::max()) noexcept
            : val (val)
        {}

        constexpr timer_id
        operator ++ ()
        {
            inc();
            return *this;
        }

        constexpr timer_id
        operator ++ (int)
        {
            const timer_id tmp = *this;
            inc();
            return tmp;
        }

        constexpr bool
        operator == (const timer_id & other) const
        {
            return val == other.val;
        }

        constexpr bool
        operator != (const timer_id & other) const
        {
            return val != other.val;
        }

        constexpr
        operator uint64_t () const
        {
            return val;
        }
    };

}

#endif //NX_ASIO_BACKEND_TYPES_HPP