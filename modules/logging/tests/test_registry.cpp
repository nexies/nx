//
// Tests for logging registry and logger wiring.
//

#include <nx/logging/logger.hpp>
#include <nx/logging/pattern_formatter.hpp>
#include <nx/logging/registry.hpp>
#include <nx/logging/sinks/sink.hpp>

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <vector>

using namespace nx::logging;

namespace {

struct capture_sink final : sink {
    std::vector<std::string> lines;

    void
    write(level msg_level, std::string_view formatted_line) override
    {
        if (!should_log(msg_level))
            return;
        lines.emplace_back(formatted_line.data(), formatted_line.size());
    }

    void
    flush() override {}
};

} // namespace

TEST_CASE("registry: get missing logger returns nullptr", "[logging][registry]")
{
    REQUIRE(get("no_such_logger") == nullptr);
}

TEST_CASE("registry: register and get round-trip", "[logging][registry]")
{
    auto cap = std::make_shared<capture_sink>();
    auto lg = std::make_shared<logger>("named", cap);
    register_logger(lg);
    REQUIRE(get("named") == lg);
}

TEST_CASE("registry: expired weak entry is dropped on get", "[logging][registry]")
{
    auto cap = std::make_shared<capture_sink>();
    {
        auto lg = std::make_shared<logger>("temp", cap);
        register_logger(lg);
    }
    REQUIRE(get("temp") == nullptr);
}

TEST_CASE("registry: default logger", "[logging][registry]")
{
    auto cap = std::make_shared<capture_sink>();
    auto lg = std::make_shared<logger>("main", cap);
    set_default_logger(lg);
    REQUIRE(get_default_logger() == lg);

    auto fmt = std::make_unique<pattern_formatter>();
    fmt->set_pattern("%v");
    lg->set_formatter(std::move(fmt));

    lg->log(level::info, "ping");
    REQUIRE(cap->lines.size() == 1);
    REQUIRE(cap->lines[0] == "ping");

    set_default_logger(nullptr);
    REQUIRE(get_default_logger() == nullptr);
}
