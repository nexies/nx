//
// Created by nexie on 18.04.2026.
//

#ifndef NX_LOGGER_MESSAGE_HPP
#define NX_LOGGER_MESSAGE_HPP
#include <string_view>

#include <nx/logging/levels.hpp>
#include <nx/logging/types.hpp>

namespace nx::logging {
    struct log_message {
        string_view_t logger_name;
        string_view_t payload;
        level_t log_level;
        source_location_t location;
        time_point_t time_point;
        tid_t tid;

        log_message();

        log_message(
            string_view_t logger_name,
            string_view_t payload,
            level_t log_level,
            source_location_t location,
            time_point_t time_point,
            tid_t tid
        );

        log_message(
            string_view_t logger_name,
            string_view_t payload,
            level_t log_level,
            source_location_t location
        );
    };
}

#endif //NX_LOGGER_MESSAGE_HPP
