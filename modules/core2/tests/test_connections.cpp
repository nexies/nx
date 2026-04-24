//
// test_connections — contracts for connect/disconnect flags and connection_flags.
//

#include <nx/core2/object.hpp>
#include <nx/common/types/enum_flags.hpp>

#include <catch2/catch_test_macros.hpp>

namespace {

struct source : nx::core::object {
    NX_OBJECT(source)
    NX_SIGNAL(fired)
    NX_SIGNAL(value_fired, int)

    using nx::core::object::object;
};

struct sink : nx::core::object {
    NX_OBJECT(sink)

    int fire_count  { 0 };
    int last_value  { -1 };

    void on_fire()        { ++fire_count; }
    void on_value(int v)  { last_value = v; ++fire_count; }

    using nx::core::object::object;
};

} // namespace

// ── connect / disconnect ──────────────────────────────────────────────────────

TEST_CASE("connections: connect returns true for new connection", "[connections]")
{
    source src;
    sink   dst;

    const bool ok = nx::core::connect(&src, &source::fired, &dst, &sink::on_fire);
    REQUIRE(ok);
}

TEST_CASE("connections: disconnect removes slot from dispatch", "[connections]")
{
    source src;
    sink   dst;

    nx::core::connect(&src, &source::fired, &dst, &sink::on_fire);
    src.fired();
    REQUIRE(dst.fire_count == 1);

    nx::core::disconnect(&src, &source::fired, &dst, &sink::on_fire);
    src.fired();
    REQUIRE(dst.fire_count == 1); // still 1 — slot not called after disconnect
}

TEST_CASE("connections: disconnect returns true when connection existed", "[connections]")
{
    source src;
    sink   dst;

    nx::core::connect(&src, &source::fired, &dst, &sink::on_fire);
    const bool removed = nx::core::disconnect(&src, &source::fired, &dst, &sink::on_fire);
    REQUIRE(removed);
}

TEST_CASE("connections: disconnect returns false when connection never existed", "[connections]")
{
    source src;
    sink   dst;

    const bool removed = nx::core::disconnect(&src, &source::fired, &dst, &sink::on_fire);
    REQUIRE_FALSE(removed);
}

// ── unique flag ───────────────────────────────────────────────────────────────

TEST_CASE("connections: unique flag prevents duplicate connections", "[connections]")
{
    source src;
    sink   dst;

    const bool first  = nx::core::connect_unique(&src, &source::fired, &dst, &sink::on_fire);
    const bool second = nx::core::connect_unique(&src, &source::fired, &dst, &sink::on_fire);

    REQUIRE(first);
    REQUIRE_FALSE(second); // duplicate rejected

    src.fired();
    REQUIRE(dst.fire_count == 1); // only fired once
}

TEST_CASE("connections: without unique flag duplicate connection fires twice", "[connections]")
{
    source src;
    sink   dst;

    nx::core::connect(&src, &source::fired, &dst, &sink::on_fire);
    nx::core::connect(&src, &source::fired, &dst, &sink::on_fire);

    src.fired();
    REQUIRE(dst.fire_count == 2);
}

// ── single_shot flag ──────────────────────────────────────────────────────────

TEST_CASE("connections: single_shot connection fires exactly once then auto-disconnects", "[connections]")
{
    source src;
    sink   dst;

    nx::core::connect(&src, &source::fired, &dst, &sink::on_fire,
                      nx::core::connection_type::direct,
                      nx::core::connection_flag::single_shot);

    src.fired();
    src.fired();
    src.fired();

    REQUIRE(dst.fire_count == 1);
}

// ── connection_flags bitwise operations ───────────────────────────────────────

TEST_CASE("connection_flags: default-constructed flags are empty", "[connection_flags]")
{
    nx::core::connection_flags f;
    REQUIRE_FALSE(f.has(nx::core::connection_flag::unique));
    REQUIRE_FALSE(f.has(nx::core::connection_flag::single_shot));
}

TEST_CASE("connection_flags: set and test a flag", "[connection_flags]")
{
    nx::core::connection_flags f;
    f.set(nx::core::connection_flag::unique);

    REQUIRE(f.has(nx::core::connection_flag::unique));
    REQUIRE_FALSE(f.has(nx::core::connection_flag::single_shot));
}

TEST_CASE("connection_flags: can combine multiple flags", "[connection_flags]")
{
    nx::core::connection_flags f =
        nx::core::connection_flag::unique | nx::core::connection_flag::single_shot;

    REQUIRE(f.has(nx::core::connection_flag::unique));
    REQUIRE(f.has(nx::core::connection_flag::single_shot));
}

TEST_CASE("connection_flags: unset clears an individual flag", "[connection_flags]")
{
    nx::core::connection_flags f =
        nx::core::connection_flag::unique | nx::core::connection_flag::single_shot;

    f.unset(nx::core::connection_flag::unique);

    REQUIRE_FALSE(f.has(nx::core::connection_flag::unique));
    REQUIRE(f.has(nx::core::connection_flag::single_shot));
}

TEST_CASE("connection_flags: operator& produces intersection", "[connection_flags]")
{
    using F = nx::core::connection_flag;

    nx::core::connection_flags both = F::unique | F::single_shot;
    nx::core::connection_flags one  = F::unique;

    nx::core::connection_flags result = both & one;

    REQUIRE(result.has(F::unique));
    REQUIRE_FALSE(result.has(F::single_shot));
}

// ── Receiver destruction auto-disconnects ────────────────────────────────────

TEST_CASE("connections: receiver destruction auto-removes connection", "[connections]")
{
    source src;

    {
        sink dst;
        nx::core::connect(&src, &source::fired, &dst, &sink::on_fire);
        src.fired();
        REQUIRE(dst.fire_count == 1);
    } // dst destroyed — notify_senders_of_destruction() removes the connection

    // Must not crash: no dangling pointer dereference
    REQUIRE_NOTHROW(src.fired());
}
