//
// Writes formatted lines to stdout (optionally with color left to pattern).
//

#ifndef NX_LOGGING_STDOUT_SINK_HPP
#define NX_LOGGING_STDOUT_SINK_HPP

#include <nx/logging/sinks/sink.hpp>

#include <cstdio>

namespace nx::logging {

class stdout_sink final : public sink {
public:
    void
    write(level msg_level, std::string_view formatted_line) override
    {
        if (!should_log(msg_level))
            return;
        if (!formatted_line.empty())
            std::fwrite(formatted_line.data(), 1, formatted_line.size(), stdout);
        std::fputc('\n', stdout);
    }

    void
    flush() override
    {
        std::fflush(stdout);
    }
};

} // namespace nx::logging

#endif // NX_LOGGING_STDOUT_SINK_HPP
