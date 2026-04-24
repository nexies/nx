//
// Pattern formatter — compiles pattern into format steps at set_pattern().
//

#include <nx/logging/pattern_formatter.hpp>

#include <nx/common/format/fmt/format.h>

#include <chrono>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <sstream>
#include <thread>

#include <cstring>

namespace nx::logging {

namespace {

[[nodiscard]] const char*
ansi_color_begin(level l) noexcept
{
    switch (l) {
    case level::trace:
        return "\x1b[38;5;8m";
    case level::debug:
        return "\x1b[38;5;6m";
    case level::info:
        return "\x1b[38;5;2m";
    case level::warn:
        return "\x1b[38;5;3m";
    case level::err:
        return "\x1b[38;5;1m";
    case level::critical:
        return "\033[1m\033[31m";
    default:
        return "";
    }
}

void
append_local_datetime_field(log_message const& msg, char spec, memory_buffer_t& buf)
{
    using namespace std::chrono;
    auto const tt = system_clock::to_time_t(msg.time_point);
    std::tm tm_buf {};

#if defined(_WIN32)
    localtime_s(&tm_buf, &tt);
#else
    localtime_r(&tt, &tm_buf);
#endif

    if (spec == 'Y') {
        fmt::format_to(std::back_inserter(buf), "{:04}", tm_buf.tm_year + 1900);
        return;
    }
    if (spec == 'm') {
        fmt::format_to(std::back_inserter(buf), "{:02}", tm_buf.tm_mon + 1);
        return;
    }
    if (spec == 'd') {
        fmt::format_to(std::back_inserter(buf), "{:02}", tm_buf.tm_mday);
        return;
    }
    if (spec == 'H') {
        fmt::format_to(std::back_inserter(buf), "{:02}", tm_buf.tm_hour);
        return;
    }
    if (spec == 'M') {
        fmt::format_to(std::back_inserter(buf), "{:02}", tm_buf.tm_min);
        return;
    }
    if (spec == 'S') {
        fmt::format_to(std::back_inserter(buf), "{:02}", tm_buf.tm_sec);
        return;
    }
    if (spec == 'f') {
        auto const us = duration_cast<microseconds>(msg.time_point.time_since_epoch()).count() % 1000000;
        if (us < 0) {
            fmt::format_to(std::back_inserter(buf), "{:06}", 0);
        } else {
            fmt::format_to(std::back_inserter(buf), "{:06}", static_cast<int>(us));
        }
        return;
    }
}

} // namespace

void
pattern_formatter::add_custom_flag(char c, custom_flag_fn fn)
{
    custom_flags_[c] = std::move(fn);
}

void
pattern_formatter::set_pattern(std::string_view p)
{
    steps_.clear();
    for (size_t i = 0; i < p.size();) {
        if (p[i] != '%') {
            size_t j = i;
            while (j < p.size() && p[j] != '%')
                ++j;
            std::string lit { p.substr(i, j - i) };
            steps_.push_back([lit = std::move(lit)](log_message const&, memory_buffer_t& buf) {
                buf.append(lit.data(), lit.data() + lit.size());
            });
            i = j;
            continue;
        }
        ++i;
        if (i >= p.size())
            break;
        if (p[i] == '%') {
            steps_.push_back([](log_message const&, memory_buffer_t& buf) { buf.push_back('%'); });
            ++i;
            continue;
        }

        char const f = p[i++];

        if (auto it = custom_flags_.find(f); it != custom_flags_.end()) {
            custom_flag_fn fn = it->second;
            steps_.push_back([fn = std::move(fn)](log_message const& msg, memory_buffer_t& buf) {
                fn(msg, buf);
            });
            continue;
        }

        switch (f) {
        case 'n':
            steps_.push_back([](log_message const& msg, memory_buffer_t& buf) {
                buf.append(msg.logger_name.data(), msg.logger_name.data() + msg.logger_name.size());
            });
            break;
        case 'v':
            steps_.push_back([](log_message const& msg, memory_buffer_t& buf) {
                buf.append(msg.payload.data(), msg.payload.data() + msg.payload.size());
            });
            break;
        case 'l':
            steps_.push_back([](log_message const& msg, memory_buffer_t& buf) {
                auto const s = to_short_c_str(msg.log_level);
                buf.append(s, s + std::strlen(s));
            });
            break;
        case 'L':
            steps_.push_back([](log_message const& msg, memory_buffer_t& buf) {
                auto const sv = to_string_view(msg.log_level);
                buf.append(sv.data(), sv.data() + sv.size());
            });
            break;
        case 's':
            steps_.push_back([](log_message const& msg, memory_buffer_t& buf) {
                std::filesystem::path const p { msg.location.file() };
                auto const fn = p.filename().string();
                buf.append(fn.data(), fn.data() + fn.size());
            });
            break;
        case '#':
            steps_.push_back([](log_message const& msg, memory_buffer_t& buf) {
                fmt::format_to(std::back_inserter(buf), "{}", msg.location.line());
            });
            break;
        case 't':
            steps_.push_back([](log_message const&, memory_buffer_t& buf) {
                std::ostringstream oss;
                oss << std::this_thread::get_id();
                auto const s = oss.str();
                buf.append(s.data(), s.data() + s.size());
            });
            break;
        case '^':
            steps_.push_back([](log_message const& msg, memory_buffer_t& buf) {
                auto const c = ansi_color_begin(msg.log_level);
                buf.append(c, c + std::strlen(c));
            });
            break;
        case '$':
            steps_.push_back([](log_message const&, memory_buffer_t& buf) {
                static constexpr char reset[] = "\033[0m";
                buf.append(reset, reset + sizeof(reset) - 1);
            });
            break;
        case 'Y':
        case 'm':
        case 'd':
        case 'H':
        case 'M':
        case 'S':
        case 'f': {
            char const spec = f;
            steps_.push_back([spec](log_message const& msg, memory_buffer_t& buf) {
                append_local_datetime_field(msg, spec, buf);
            });
            break;
        }
        default: {
            std::string bad;
            bad.push_back('%');
            bad.push_back(f);
            steps_.push_back([bad = std::move(bad)](log_message const&, memory_buffer_t& buf) {
                buf.append(bad.data(), bad.data() + bad.size());
            });
            break;
        }
        }
    }
}

void
pattern_formatter::format(log_message const& msg, memory_buffer_t& dest) const
{
    for (auto const& step : steps_)
        step(msg, dest);
}

} // namespace nx::logging
