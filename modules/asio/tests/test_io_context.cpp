//
// Created by nexie on 4/7/2026.
//

#include <catch2/catch_test_macros.hpp>

#include <nx/asio/io_context.hpp>

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

TEST_CASE("io_context: default state", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;

    REQUIRE_FALSE(ctx.stopped());
    REQUIRE_FALSE(ctx.is_running_in_this_thread());
}

TEST_CASE("io_context: post schedules task executed by poll", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;
    bool called = false;

    ctx.post([&]() {
        called = true;
    });

    const auto n = ctx.poll();

    REQUIRE(n == 1);
    REQUIRE(called);
}

TEST_CASE("io_context: poll executes all queued tasks", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;
    int sum = 0;

    ctx.post([&]() { sum += 1; });
    ctx.post([&]() { sum += 2; });
    ctx.post([&]() { sum += 3; });

    const auto n = ctx.poll();

    REQUIRE(n == 3);
    REQUIRE(sum == 6);
}

TEST_CASE("io_context: poll_once executes only one queued task", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;
    int counter = 0;

    ctx.post([&]() { ++counter; });
    ctx.post([&]() { ++counter; });

    const auto n = ctx.poll_once();

    REQUIRE(n == 1);
    REQUIRE(counter == 1);
}

TEST_CASE("io_context: poll on empty context does nothing", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;

    REQUIRE(ctx.poll() == 0);
    REQUIRE(ctx.poll_once() == 0);
}

TEST_CASE("io_context: dispatch outside run queues work", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;
    bool called = false;

    ctx.dispatch([&]() {
        called = true;
    });

    REQUIRE_FALSE(called);

    const auto n = ctx.poll();

    REQUIRE(n == 1);
    REQUIRE(called);
}

TEST_CASE("io_context: dispatch inside run executes immediately in same thread", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;
    std::vector<int> order;

    ctx.post([&]() {
        order.push_back(1);

        ctx.dispatch([&]() {
            order.push_back(2);
            REQUIRE(ctx.is_running_in_this_thread());
        });

        order.push_back(3);
        ctx.stop();
    });

    const auto n = ctx.run();

    REQUIRE(n >= 1);
    REQUIRE(order == std::vector<int>{1, 2, 3});
}

TEST_CASE("io_context: is_running_in_this_thread is true inside run handler", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;
    bool inside = false;
    bool outside_before = ctx.is_running_in_this_thread();
    bool outside_after = false;

    ctx.post([&]() {
        inside = ctx.is_running_in_this_thread();
        ctx.stop();
    });

    ctx.run();
    outside_after = ctx.is_running_in_this_thread();

    REQUIRE_FALSE(outside_before);
    REQUIRE(inside);
    REQUIRE_FALSE(outside_after);
}

TEST_CASE("io_context: run processes tasks until stop is called", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;
    int counter = 0;

    ctx.post([&]() {
        ++counter;
    });

    ctx.post([&]() {
        ++counter;
        ctx.stop();
    });

    const auto n = ctx.run();

    REQUIRE(n >= 2);
    REQUIRE(counter == 2);
    REQUIRE(ctx.stopped());
}

TEST_CASE("io_context: stop causes run to exit even without queued work", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;

    std::thread stopper([&]() {
        std::this_thread::sleep_for(10ms);
        ctx.stop();
    });

    const auto n = ctx.run();

    stopper.join();

    REQUIRE(n == 0);
    REQUIRE(ctx.stopped());
}

TEST_CASE("io_context: restart allows run to be used again after stop", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;
    int counter = 0;

    ctx.post([&]() {
        ++counter;
        ctx.stop();
    });

    ctx.run();

    REQUIRE(ctx.stopped());
    REQUIRE(counter == 1);

    ctx.restart();

    REQUIRE_FALSE(ctx.stopped());

    ctx.post([&]() {
        ++counter;
        ctx.stop();
    });

    const auto n = ctx.run();

    REQUIRE(n >= 1);
    REQUIRE(counter == 2);
    REQUIRE(ctx.stopped());
}

TEST_CASE("io_context: run_for returns when stop is called", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;
    bool called = false;

    ctx.post([&]() {
        called = true;
        ctx.stop();
    });

    const auto n = ctx.run_for(100ms);

    REQUIRE(n >= 1);
    REQUIRE(called);
    REQUIRE(ctx.stopped());
}

TEST_CASE("io_context: run_until returns when stop is called", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;
    bool called = false;

    ctx.post([&]() {
        called = true;
        ctx.stop();
    });

    const auto n = ctx.run_until(std::chrono::system_clock::now() + 100ms);

    REQUIRE(n >= 1);
    REQUIRE(called);
    REQUIRE(ctx.stopped());
}

TEST_CASE("io_context: tasks posted from another thread are executed by run", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;
    std::atomic<bool> called = false;

    std::thread worker([&]() {
        std::this_thread::sleep_for(10ms);
        ctx.post([&]() {
            called = true;
            ctx.stop();
        });
    });

    const auto n = ctx.run();

    worker.join();

    REQUIRE(n >= 1);
    REQUIRE(called.load());
    REQUIRE(ctx.stopped());
}

TEST_CASE("io_context: nested post is queued and executed by poll", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;
    std::vector<int> order;

    ctx.post([&]() {
        order.push_back(1);
        ctx.post([&]() {
            order.push_back(3);
        });
        order.push_back(2);
    });

    const auto first = ctx.poll();
    const auto second = ctx.poll();

    REQUIRE(first == 1);
    REQUIRE(second == 1);
    REQUIRE(order == std::vector<int>{1, 2, 3});
}

TEST_CASE("io_context: run can be stopped from another thread", "[nx::asio][io_context]")
{
    nx::asio::io_context ctx;

    std::thread stopper([&]() {
        std::this_thread::sleep_for(10ms);
        ctx.stop();
    });

    const auto n = ctx.run();

    stopper.join();

    REQUIRE(n == 0);
    REQUIRE(ctx.stopped());
}