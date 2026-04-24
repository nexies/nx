//
// Tests for pattern_formatter (spdlog-style flags subset).
//

#include <nx/common/format/fmt/format.h>
#include <nx/logging/message.hpp>
#include <nx/logging/pattern_formatter.hpp>
#include <nx/logging/types.hpp>

#include <catch2/catch_test_macros.hpp>

#include <iterator>
#include <string>

using namespace nx::logging;

namespace {

log_message
make_message(
    std::string const& logger_name,
    std::string payload,
    level lvl,
    time_point_t tp = {}
)
{
    return {
        std::string_view { logger_name },
        std::move(payload),
        lvl,
        nx::g_undefined_location,
        tp,
    };
}

std::string
format_to_string(pattern_formatter const& fmt, log_message const& msg)
{
    memory_buffer_t buf;
    fmt.format(msg, buf);
    return std::string(buf.data(), buf.size());
}

} // namespace

TEST_CASE("pattern: literal and escape %%", "[logging][pattern]")
{
    pattern_formatter fmt;
    fmt.set_pattern("pre %% post");
    auto const out = format_to_string(fmt, make_message("n", "x", level::info));
    REQUIRE(out == "pre % post");
}

TEST_CASE("pattern: %n and %v", "[logging][pattern]")
{
    pattern_formatter fmt;
    fmt.set_pattern("[%n] %v");
    auto const out = format_to_string(fmt, make_message("core", "hello", level::warn));
    REQUIRE(out == "[core] hello");
}

TEST_CASE("pattern: %l short level", "[logging][pattern]")
{
    pattern_formatter fmt;
    fmt.set_pattern("%l");
    auto const out = format_to_string(fmt, make_message("n", "p", level::err));
    REQUIRE(out == "E");
}

TEST_CASE("pattern: %L long level", "[logging][pattern]")
{
    pattern_formatter fmt;
    fmt.set_pattern("%L");
    auto const out = format_to_string(fmt, make_message("n", "p", level::warn));
    REQUIRE(out == "warning");
}

TEST_CASE("pattern: custom flag", "[logging][pattern]")
{
    pattern_formatter fmt;
    fmt.add_custom_flag('X', [](log_message const&, memory_buffer_t& buf) {
        fmt::format_to(std::back_inserter(buf), "{}", 42);
    });
    fmt.set_pattern("x%Xy");
    auto const out = format_to_string(fmt, make_message("n", "p", level::info));
    REQUIRE(out == "x42y");
}

TEST_CASE("pattern: unknown flag emitted as literal", "[logging][pattern]")
{
    pattern_formatter fmt;
    fmt.set_pattern("%?");
    auto const out = format_to_string(fmt, make_message("n", "p", level::info));
    REQUIRE(out == "%?");
}

TEST_CASE("pattern: add_custom_flag then set_pattern", "[logging][pattern]")
{
    pattern_formatter fmt;
    fmt.add_custom_flag('T', [](log_message const&, memory_buffer_t& buf) {
        static constexpr char k[] = "tid";
        buf.append(k, k + sizeof(k) - 1);
    });
    fmt.set_pattern("id=%T");
    auto const out = format_to_string(fmt, make_message("n", "p", level::trace));
    REQUIRE(out == "id=tid");
}
