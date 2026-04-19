//
// Created by nexie on 18.04.2026.
//

#ifndef NX_LOGGER_BASIC_SINK_HPP
#define NX_LOGGER_BASIC_SINK_HPP

#include <nx/logging/message.hpp>

namespace nx::logging {
    class basic_sink {
        level_t lower_level_ { level::trace };
        level_t higher_level_ { level::critical };
    public:
        virtual
        ~basic_sink() = default;

        virtual void
        log (log_message const& message) = 0;

        virtual void
        flush () = 0;

        virtual bool
        should_log (level_t level) {
            return lower_level_ >= level && level >= higher_level_;
        }

        void
        set_level (level_t level) {
            lower_level_ = level;
        }

        void
        set_higher_level (level_t level) {
            higher_level_ = level;
        }
    };

    using sink_ptr = std::shared_ptr<basic_sink>;
}

#endif //NX_LOGGER_BASIC_SINK_HPP
