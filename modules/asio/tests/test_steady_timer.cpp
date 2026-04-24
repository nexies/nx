//
// Created by nexie on 4/7/2026.
//

#include <catch2/catch_test_macros.hpp>

#include <nx/asio/context/io_context.hpp>
#include <nx/asio/steady_timer.hpp>

#include <atomic>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

TEST_CASE("steady_timer: new timer is not running", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer(ctx);

    REQUIRE_FALSE(timer.running());
}

TEST_CASE("steady_timer: id is stable", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer(ctx);

    const auto id1 = timer.id();
    const auto id2 = timer.id();

    REQUIRE(id1 == id2);
}

TEST_CASE("steady_timer: async_wait(duration) starts timer", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer(ctx);

    bool called = false;

    timer.async_wait(50ms, [&]() {
        called = true;
        ctx.stop();
    });

    REQUIRE(timer.running());

    const auto n = ctx.run();

    REQUIRE(n >= 1);
    REQUIRE(called);
    REQUIRE_FALSE(timer.running());
}

TEST_CASE("steady_timer: async_wait(time_point) starts timer", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer(ctx);

    bool called = false;
    const auto expiry = nx::asio::clock::now() + 50ms;

    timer.async_wait(expiry, [&]() {
        called = true;
        ctx.stop();
    });

    REQUIRE(timer.running());

    const auto n = ctx.run();

    REQUIRE(n >= 1);
    REQUIRE(called);
    REQUIRE_FALSE(timer.running());
}

TEST_CASE("steady_timer: cancel prevents callback execution", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer(ctx);

    bool called = false;

    timer.async_wait(100ms, [&]() {
        called = true;
    });

    REQUIRE(timer.running());

    timer.cancel();

    REQUIRE_FALSE(timer.running());

    ctx.run_for(150ms);

    REQUIRE_FALSE(called);
}

TEST_CASE("steady_timer: time_left is positive right after scheduling", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer(ctx);

    timer.async_wait(200ms, []() {});

    const auto left = timer.time_left();

    REQUIRE(timer.running());
    REQUIRE(left > nx::asio::duration::zero());
    REQUIRE(left <= 200ms);
}

TEST_CASE("steady_timer: time_left decreases over time", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer(ctx);

    timer.async_wait(200ms, []() {});

    const auto left1 = timer.time_left();
    std::this_thread::sleep_for(50ms);
    const auto left2 = timer.time_left();

    REQUIRE(left1 > left2);
}

TEST_CASE("steady_timer: after firing timer is no longer running", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer(ctx);

    bool called = false;

    timer.async_wait(30ms, [&]() {
        called = true;
        REQUIRE(timer.running());
        ctx.stop();
    });

    ctx.run();

    REQUIRE(called);
    REQUIRE_FALSE(timer.running());
}

TEST_CASE("steady_timer: cancel on non-running timer is harmless", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer(ctx);

    REQUIRE_FALSE(timer.running());

    timer.cancel();

    REQUIRE_FALSE(timer.running());
}

TEST_CASE("steady_timer: rescheduling replaces previous wait", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer(ctx);

    int calls = 0;

    timer.async_wait(200ms, [&]() {
        ++calls;
    });

    timer.async_wait(30ms, [&]() {
        ++calls;
        ctx.stop();
    });

    ctx.run();

    REQUIRE(calls == 1);
    REQUIRE_FALSE(timer.running());
}

TEST_CASE("steady_timer: two timers can fire independently", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer1(ctx);
    nx::asio::steady_timer timer2(ctx);

    int calls = 0;

    timer1.async_wait(20ms, [&]() {
        ++calls;
    });

    timer2.async_wait(40ms, [&]() {
        ++calls;
        ctx.stop();
    });

    const auto n = ctx.run();

    REQUIRE(n >= 2);
    REQUIRE(calls == 2);
    REQUIRE_FALSE(timer1.running());
    REQUIRE_FALSE(timer2.running());
}

TEST_CASE("steady_timer: cancelled timer does not affect another timer", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer1(ctx);
    nx::asio::steady_timer timer2(ctx);

    bool first_called = false;
    bool second_called = false;

    timer1.async_wait(100ms, [&]() {
        first_called = true;
    });

    timer2.async_wait(30ms, [&]() {
        second_called = true;
        ctx.stop();
    });

    timer1.cancel();

    ctx.run();

    REQUIRE_FALSE(first_called);
    REQUIRE(second_called);
}

TEST_CASE("steady_timer: zero duration wait executes callback", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer(ctx);

    bool called = false;

    timer.async_wait(nx::asio::duration::zero(), [&]() {
        called = true;
        ctx.stop();
    });

    const auto n = ctx.run();

    REQUIRE(n >= 1);
    REQUIRE(called);
    REQUIRE_FALSE(timer.running());
}

TEST_CASE("steady_timer: time_left for expired or stopped timer is non-positive or zero", "[nx::asio][steady_timer]")
{
    nx::asio::io_context ctx;
    nx::asio::steady_timer timer(ctx);

    timer.async_wait(20ms, [&]() {
        ctx.stop();
    });

    ctx.run();

    REQUIRE_FALSE(timer.running());
    REQUIRE(timer.time_left() <= nx::asio::duration::zero());
}