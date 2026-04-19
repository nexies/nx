//
// Base sink — receives a fully formatted line (after pattern formatting).
//

#ifndef NX_LOGGING_SINK_HPP
#define NX_LOGGING_SINK_HPP

#include <nx/logging/levels.hpp>

#include <string_view>

namespace nx::logging {

enum class color_mode {
    automatic = 0,
    always = 1,
    never = 2,
};

class sink {
public:
    virtual ~sink() = default;

    virtual void
    write(level msg_level, std::string_view formatted_line) = 0;

    virtual void
    flush() = 0;

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

    void
    set_color_mode(color_mode m) noexcept
    {
        color_mode_ = m;
    }

    [[nodiscard]] color_mode
    get_color_mode() const noexcept
    {
        return color_mode_;
    }

protected:
    level level_ { level::trace };
    color_mode color_mode_ { color_mode::automatic };
};

} // namespace nx::logging

#endif // NX_LOGGING_SINK_HPP
