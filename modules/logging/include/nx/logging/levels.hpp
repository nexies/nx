//
// Log levels — numeric order matches spdlog (trace = most verbose).
//

#ifndef NX_LOGGING_LEVELS_HPP
#define NX_LOGGING_LEVELS_HPP

#include <string_view>

namespace nx::logging {

enum class level : int {
    trace = 0,
    debug = 1,
    info = 2,
    warn = 3,
    err = 4,
    critical = 5,
    off = 6,
};

[[nodiscard]] constexpr bool
should_log_level(level message_level, level threshold) noexcept
{
    return static_cast<int>(message_level) >= static_cast<int>(threshold)
        && message_level != level::off;
}

[[nodiscard]] inline std::string_view
to_string_view(level l) noexcept
{
    switch (l) {
    case level::trace:
        return "trace";
    case level::debug:
        return "debug";
    case level::info:
        return "info";
    case level::warn:
        return "warning";
    case level::err:
        return "error";
    case level::critical:
        return "critical";
    case level::off:
        return "off";
    }
    return "unknown";
}

[[nodiscard]] inline const char *
to_short_c_str(level l) noexcept
{
    switch (l) {
    case level::trace:
        return "T";
    case level::debug:
        return "D";
    case level::info:
        return "I";
    case level::warn:
        return "W";
    case level::err:
        return "E";
    case level::critical:
        return "C";
    case level::off:
        return "O";
    }
    return "?";
}

} // namespace nx::logging

#endif // NX_LOGGING_LEVELS_HPP
