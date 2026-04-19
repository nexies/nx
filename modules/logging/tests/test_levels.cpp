//
// Tests for nx::logging level helpers.
//

#include <nx/logging/levels.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace nx::logging;

TEST_CASE("should_log_level: message at or above threshold", "[logging][levels]")
{
    REQUIRE(should_log_level(level::trace, level::trace));
    REQUIRE(should_log_level(level::info, level::trace));
    REQUIRE(should_log_level(level::critical, level::warn));
}

TEST_CASE("should_log_level: below threshold is filtered", "[logging][levels]")
{
    REQUIRE_FALSE(should_log_level(level::debug, level::info));
    REQUIRE_FALSE(should_log_level(level::trace, level::err));
}

TEST_CASE("should_log_level: off never logs", "[logging][levels]")
{
    REQUIRE_FALSE(should_log_level(level::off, level::trace));
}

TEST_CASE("to_string_view: known levels", "[logging][levels]")
{
    REQUIRE(to_string_view(level::trace) == "trace");
    REQUIRE(to_string_view(level::warn) == "warning");
    REQUIRE(to_string_view(level::err) == "error");
    REQUIRE(to_string_view(level::off) == "off");
}

TEST_CASE("to_short_c_str: single-letter codes", "[logging][levels]")
{
    REQUIRE(std::string_view(to_short_c_str(level::info)) == "I");
    REQUIRE(std::string_view(to_short_c_str(level::critical)) == "C");
}
