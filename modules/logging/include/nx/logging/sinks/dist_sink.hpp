//
// Fan-out sink — forwards the same formatted line to child sinks.
//

#ifndef NX_LOGGING_DIST_SINK_HPP
#define NX_LOGGING_DIST_SINK_HPP

#include <nx/logging/sinks/sink.hpp>

#include <memory>
#include <vector>

namespace nx::logging {

class dist_sink final : public sink {
public:
    void
    add_sink(std::shared_ptr<sink> s)
    {
        if (s)
            sinks_.push_back(std::move(s));
    }

    void
    write(level msg_level, std::string_view formatted_line) override
    {
        if (!should_log(msg_level))
            return;
        for (auto const& s : sinks_)
            s->write(msg_level, formatted_line);
    }

    void
    flush() override
    {
        for (auto const& s : sinks_)
            s->flush();
    }

    [[nodiscard]] std::vector<std::shared_ptr<sink>> const&
    sinks() const noexcept
    {
        return sinks_;
    }

private:
    std::vector<std::shared_ptr<sink>> sinks_;
};

} // namespace nx::logging

#endif // NX_LOGGING_DIST_SINK_HPP
