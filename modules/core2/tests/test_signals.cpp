//
// test_signals — contracts for the core2 signal/slot dispatch system.
//

#include <nx/core2/object/object.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

namespace {

// ── Test objects ──────────────────────────────────────────────────────────────

struct emitter : nx::core::object {
    NX_OBJECT(emitter)
    NX_SIGNAL(triggered)
    NX_SIGNAL(value_changed, int)
    NX_SIGNAL(pair_changed, int, std::string)

    using nx::core::object::object;
};

struct receiver : nx::core::object {
    int         trigger_count { 0 };
    int         last_value    { -1 };
    int         last_pair_int { -1 };
    std::string last_pair_str;

    void on_trigger()                      { ++trigger_count; }
    void on_value(int v)                   { last_value = v; }
    void on_pair(int i, std::string s)     { last_pair_int = i; last_pair_str = std::move(s); }

    using nx::core::object::object;
};

} // namespace

// ── Basic dispatch ────────────────────────────────────────────────────────────

TEST_CASE("signal: no-arg signal reaches connected slot", "[signals]")
{
    emitter  src;
    receiver dst;

    nx::core::connect(&src, &emitter::triggered, &dst, &receiver::on_trigger);
    src.triggered();

    REQUIRE(dst.trigger_count == 1);
}

TEST_CASE("signal: slot is called once per emission", "[signals]")
{
    emitter  src;
    receiver dst;

    nx::core::connect(&src, &emitter::triggered, &dst, &receiver::on_trigger);
    src.triggered();
    src.triggered();
    src.triggered();

    REQUIRE(dst.trigger_count == 3);
}

TEST_CASE("signal: int argument is forwarded correctly", "[signals]")
{
    emitter  src;
    receiver dst;

    nx::core::connect(&src, &emitter::value_changed, &dst, &receiver::on_value);
    src.value_changed(42);

    REQUIRE(dst.last_value == 42);
}

TEST_CASE("signal: multiple arguments are forwarded correctly", "[signals]")
{
    emitter  src;
    receiver dst;

    nx::core::connect(&src, &emitter::pair_changed, &dst, &receiver::on_pair);
    src.pair_changed(7, "hello");

    REQUIRE(dst.last_pair_int == 7);
    REQUIRE(dst.last_pair_str == "hello");
}

TEST_CASE("signal: multiple receivers all receive the emission", "[signals]")
{
    emitter  src;
    receiver r1, r2, r3;

    nx::core::connect(&src, &emitter::triggered, &r1, &receiver::on_trigger);
    nx::core::connect(&src, &emitter::triggered, &r2, &receiver::on_trigger);
    nx::core::connect(&src, &emitter::triggered, &r3, &receiver::on_trigger);

    src.triggered();

    REQUIRE(r1.trigger_count == 1);
    REQUIRE(r2.trigger_count == 1);
    REQUIRE(r3.trigger_count == 1);
}

TEST_CASE("signal: no-arg signal with no connections is a no-op", "[signals]")
{
    emitter src;
    REQUIRE_NOTHROW(src.triggered());
}

TEST_CASE("signal: sender() returns the emitting object during slot execution", "[signals]")
{
    emitter  src;
    nx::core::object * captured_sender = nullptr;

    struct spy : nx::core::object {
        nx::core::object ** out;
        void record() { *out = sender(); }
        explicit spy(nx::core::object ** p) : out(p) {}
    } s { &captured_sender };

    nx::core::connect(&src, &emitter::triggered, &s, &spy::record);
    src.triggered();

    REQUIRE(captured_sender == &src);
}

// ── NX_EMIT macro ─────────────────────────────────────────────────────────────

TEST_CASE("NX_EMIT: dispatches through signal method", "[signals]")
{
    // NX_EMIT(name, args...) is used inside class methods. We verify it works
    // the same way as calling the signal method directly.
    struct producer : nx::core::object {
        NX_OBJECT(producer)
        NX_SIGNAL(fired, int)

        void produce(int v) { NX_EMIT(fired, v); }
    };

    producer     p;
    receiver     r;

    nx::core::connect(&p, &producer::fired, &r, &receiver::on_value);
    p.produce(99);

    REQUIRE(r.last_value == 99);
}
