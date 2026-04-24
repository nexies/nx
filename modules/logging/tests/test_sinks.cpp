//
// Tests for sinks (dist fan-out, level gating).
//

#include <nx/logging/levels.hpp>
#include <nx/logging/sinks/dist_sink.hpp>
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

TEST_CASE("dist_sink: forwards to all children", "[logging][sinks]")
{
    auto a = std::make_shared<capture_sink>();
    auto b = std::make_shared<capture_sink>();
    auto d = std::make_shared<dist_sink>();
    d->set_level(level::trace);
    d->add_sink(a);
    d->add_sink(b);

    d->write(level::info, "one line");

    REQUIRE(a->lines.size() == 1);
    REQUIRE(b->lines.size() == 1);
    REQUIRE(a->lines[0] == "one line");
    REQUIRE(b->lines[0] == "one line");
}

TEST_CASE("dist_sink: respects sink level", "[logging][sinks]")
{
    auto inner = std::make_shared<capture_sink>();
    inner->set_level(level::warn);

    auto d = std::make_shared<dist_sink>();
    d->set_level(level::trace);
    d->add_sink(inner);

    inner->write(level::info, "blocked");
    REQUIRE(inner->lines.empty());

    inner->write(level::err, "shown");
    REQUIRE(inner->lines.size() == 1);
    REQUIRE(inner->lines[0] == "shown");
}
