//
// Created by nexie on 18.04.2026.
//

#ifndef NX_LOGGING_LOGGER_HPP
#define NX_LOGGING_LOGGER_HPP

#include <nx/logging/types.hpp>
#include <nx/logging/sinks/basic_sink.hpp>

#include <vector>
#include "levels.hpp"

namespace nx::logging {

    class logger {
        std::string name_;
        std::vector<sink_ptr> sinks_;
        level_t lower_level_ { level::trace };
        level_t higher_level_ { level::critical };

        public:

        template<typename ... Args> void
        log(level_t level, source_location_t loc, string_view_t fmt, Args... args) {
            log_message m(
                name_,
                fmt::format(fmt, args...),
                level, loc,
                clock_t::now(),
                0);
            for (auto & sink : sinks_) {
                sink->log(m);
            }
        }

        template<typename ... Args> void
        log(level_t level, string_view_t fmt, Args... args) {
            log_message m(
                name_,
                fmt::format(fmt, args...),
                level,
                g_undefined_location,
                clock_t::now(),
                0);
            for (auto & sink : sinks_) {
                sink->log(m);
            }
        }

        template<typename ... Args> void
        critical(source_location_t loc, string_view_t fmt, Args... args) {
            log(level::critical, loc, fmt::format(fmt, args...));
        }

        template<typename ... Args> void
        error(source_location_t loc, string_view_t fmt, Args... args) {
            log(level::error, loc, fmt::format(fmt, args...));
        }

        template<typename ... Args> void
        warning(source_location_t loc, string_view_t fmt, Args... args) {
            log(level::warning, loc, fmt::format(fmt, args...));
        }

        template<typename ... Args> void
        info(source_location_t loc, string_view_t fmt, Args... args) {
            log(level::info, loc, fmt::format(fmt, args...));
        }

        template<typename ... Args> void
        debug(source_location_t loc, string_view_t fmt, Args... args) {
            log(level::debug, loc, fmt::format(fmt, args...));
        }

        template<typename ... Args> void
        trace(source_location_t loc, string_view_t fmt, Args... args) {
            log(level::trace, loc, fmt::format(fmt, args...));
        }
    };


}

#endif //NX_LOGGING_LOGGER_HPP
