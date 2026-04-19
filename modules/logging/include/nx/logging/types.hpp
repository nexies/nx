//
// Created by nexie on 18.04.2026.
//

#ifndef NX_LOGGER_TYPES_HPP
#define NX_LOGGER_TYPES_HPP

#include <string_view>
#include <nx/common/types/source_location.hpp>
#include <nx/logging/levels.hpp>

namespace nx::logging {

    using string_view_t = std::string_view;
    using memory_buffer_t = fmt::basic_memory_buffer<char, 512>;
    using source_location_t = nx::source_location;
    using clock_t = std::chrono::system_clock;
    using time_point_t = std::chrono::time_point<std::chrono::system_clock>;
    using level_t = level;
    using tid_t = size_t;
}

#endif //NX_LOGGER_TYPES_HPP
