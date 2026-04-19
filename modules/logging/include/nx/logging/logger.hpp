//
// Logger — formats a line once, dispatches to sinks.
//

#ifndef NX_LOGGING_LOGGER_HPP
#define NX_LOGGING_LOGGER_HPP

#include <nx/common/format/fmt/format.h>
#include <nx/common/types/source_location.hpp>
#include <nx/logging/message.hpp>
#include <nx/logging/pattern_formatter.hpp>
#include <nx/logging/sinks/sink.hpp>

#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace nx::logging {

class logger {
public:
    logger(std::string name, std::vector<std::shared_ptr<sink>> sinks)
        : name_ { std::move(name) }
        , sinks_ { std::move(sinks) }
    {}

    explicit
    logger(std::string name, std::shared_ptr<sink> single_sink)
        : name_ { std::move(name) }
    {
        sinks_.push_back(std::move(single_sink));
    }

    void
    set_level(level l) noexcept
    {
        level_ = l;
    }

    [[nodiscard]] level
    get_level() const noexcept
    {
        return level_;
    }

    [[nodiscard]] bool
    should_log(level msg_level) const noexcept
    {
        return should_log_level(msg_level, level_);
    }

    [[nodiscard]] std::string const&
    name() const noexcept
    {
        return name_;
    }

    void
    set_formatter(std::unique_ptr<pattern_formatter> fmt)
    {
        formatter_ = std::move(fmt);
    }

    template<typename... Args>
    void
    log(level lvl, nx::source_location loc, fmt::string_view fmtstr, Args&&... args)
    {
        if (!should_log(lvl))
            return;
        std::string payload = fmt::format(fmtstr, std::forward<Args>(args)...);
        log_message msg {
            std::string_view { name_ },
            std::move(payload),
            lvl,
            loc,
            std::chrono::system_clock::now(),
        };
        memory_buffer_t buf;
        if (formatter_)
            formatter_->format(msg, buf);
        else
            fmt::format_to(std::back_inserter(buf), "{}", msg.payload);

        std::string_view const line { buf.data(), buf.size() };
        std::lock_guard<std::mutex> lock { mutex_ };
        for (auto const& s : sinks_) {
            if (s && s->should_log(lvl))
                s->write(lvl, line);
        }
    }

    template<typename... Args>
    void
    log(level lvl, fmt::string_view fmtstr, Args&&... args)
    {
        log(lvl, nx::g_undefined_location, fmtstr, std::forward<Args>(args)...);
    }

    void
    flush()
    {
        std::lock_guard<std::mutex> lock { mutex_ };
        for (auto const& s : sinks_) {
            if (s)
                s->flush();
        }
    }

private:
    std::string name_;
    level level_ { level::trace };
    std::vector<std::shared_ptr<sink>> sinks_;
    std::unique_ptr<pattern_formatter> formatter_;
    mutable std::mutex mutex_;
};

} // namespace nx::logging

#endif // NX_LOGGING_LOGGER_HPP
