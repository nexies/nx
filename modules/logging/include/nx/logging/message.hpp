//
// One log line payload and metadata (before pattern formatting).
//

#ifndef NX_LOGGER_MESSAGE_HPP
#define NX_LOGGER_MESSAGE_HPP

#include <chrono>
#include <string>
#include <string_view>

#include <nx/logging/types.hpp>

namespace nx::logging {

struct log_message {
    std::string_view logger_name {};
    std::string payload {};
    level log_level { level::trace };
    nx::source_location location { nx::g_undefined_location };
    time_point_t time_point {};

    log_message() = default;

    log_message(
        std::string_view logger_name_,
        std::string payload_,
        level log_level_,
        nx::source_location location_,
        time_point_t time_point_
    )
        : logger_name { logger_name_ }
        , payload { std::move(payload_) }
        , log_level { log_level_ }
        , location { location_ }
        , time_point { time_point_ }
    {}
};

} // namespace nx::logging

#endif // NX_LOGGER_MESSAGE_HPP
