//
// Pattern-based line formatter (subset of spdlog-style flags).
//

#ifndef NX_LOGGING_PATTERN_FORMATTER_HPP
#define NX_LOGGING_PATTERN_FORMATTER_HPP

#include <nx/logging/message.hpp>
#include <nx/logging/types.hpp>

#include <functional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace nx::logging {

class pattern_formatter {
public:
    using custom_flag_fn = std::function<void(log_message const&, memory_buffer_t&)>;

    pattern_formatter() = default;

    void
    set_pattern(std::string_view pattern);

    void
    add_custom_flag(char c, custom_flag_fn fn);

    void
    format(log_message const& msg, memory_buffer_t& dest) const;

private:
    using format_step = std::function<void(log_message const&, memory_buffer_t&)>;
    std::vector<format_step> steps_;
    std::unordered_map<char, custom_flag_fn> custom_flags_;
};

} // namespace nx::logging

#endif // NX_LOGGING_PATTERN_FORMATTER_HPP
