//
// Created by nexie on 18.04.2026.
//

#ifndef NX_LOGGING_ANSI_CONSOLE_SINK_HPP
#define NX_LOGGING_ANSI_CONSOLE_SINK_HPP

#include <nx/logging/formatters/colored_ansi_formatter.hpp>
#include <nx/logging/sinks/formatted_sink.hpp>

#include <cstdio>
#include <iostream>


namespace nx::logging {
    template<typename Mutex>
    class ansi_console_sink : public formatted_sink<Mutex> {

    public:
        ansi_console_sink()
            : formatted_sink<Mutex>(std::make_unique<colored_ansi_formatter>())
        {}

    protected:
        void
        log_impl (string_view_t str) final {
            std::fprintf(stdout, "%s", str.data());
        }

        void
        flush_impl () final {
            std::fflush(stdout);
        }
    };
}

#endif //NX_LOGGING_ANSI_CONSOLE_SINK_HPP
