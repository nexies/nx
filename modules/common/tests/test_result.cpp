//
// Created by nexie on 06.04.2026.
//

#include <iostream>
#include <nx/common/types/result.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <utility>

namespace
{
    using test_result = nx::basic_result<int, std::string>;
}

TEST_CASE("basic_result: default constructor creates value", "[basic_result]")
{
    test_result result;

    REQUIRE(result.has_value());
    REQUIRE_FALSE(result.is_error());
    REQUIRE(static_cast<bool>(result));

    REQUIRE(result.value() == 0);
    REQUIRE(result.value_or(42) == 0);
}

TEST_CASE("basic_result: construct from value", "[basic_result]")
{
    test_result result(123);

    REQUIRE(result.has_value());
    REQUIRE_FALSE(result.is_error());
    REQUIRE(static_cast<bool>(result));

    REQUIRE(result.value() == 123);
    REQUIRE(result.value_or(42) == 123);
}

TEST_CASE("basic_result: construct from lvalue value", "[basic_result]")
{
    int value = 77;
    test_result result(value);

    REQUIRE(result.has_value());
    REQUIRE(result.value() == 77);
}

TEST_CASE("basic_result: construct from error", "[basic_result]")
{
    test_result result(std::string("some error"));

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.is_error());
    REQUIRE_FALSE(static_cast<bool>(result));

    REQUIRE(result.error() == "some error");
    REQUIRE(result.value_or(42) == 42);
}

TEST_CASE("basic_result: construct from lvalue error", "[basic_result]")
{
    std::string error = "lvalue error";
    test_result result(error);

    REQUIRE(result.is_error());
    REQUIRE(result.error() == "lvalue error");
}

TEST_CASE("basic_result: value throws when result contains error", "[basic_result]")
{
    test_result result(std::string("boom"));

    REQUIRE_THROWS_AS(result.value(), nx::err::invalid_argument);
}

TEST_CASE("basic_result: error throws when result contains value", "[basic_result]")
{
    test_result result(10);

    REQUIRE_THROWS_AS(result.error(), nx::err::invalid_argument);
}

TEST_CASE("basic_result: expect returns contained value when no error", "[basic_result]")
{
    test_result result(55);

    bool handler_called = false;

    auto actual = result.expect([&handler_called](const std::string&) {
        handler_called = true;
        return -1;
    });

    REQUIRE(actual == 55);
    REQUIRE_FALSE(handler_called);
}

TEST_CASE("basic_result: expect calls handler when error is present", "[basic_result]")
{
    test_result result(std::string("failure"));

    bool handler_called = false;

    auto actual = result.expect([&handler_called](const std::string& err) {
        handler_called = true;
        REQUIRE(err == "failure");
        return -100;
    });

    REQUIRE(handler_called);
    REQUIRE(actual == -100);
}

TEST_CASE("basic_result: implicit bool conversion reflects state", "[basic_result]")
{
    test_result ok(1);
    test_result fail(std::string("err"));

    REQUIRE(static_cast<bool>(ok));
    REQUIRE_FALSE(static_cast<bool>(fail));
}

TEST_CASE("basic_result: copy constructor preserves value state", "[basic_result]")
{
    test_result original(999);
    test_result copy(original);

    REQUIRE(copy.has_value());
    REQUIRE(copy.value() == 999);
}

TEST_CASE("basic_result: copy constructor preserves error state", "[basic_result]")
{
    test_result original(std::string("copy error"));
    test_result copy(original);

    REQUIRE(copy.is_error());
    REQUIRE(copy.error() == "copy error");
}

TEST_CASE("basic_result: move constructor preserves value state", "[basic_result]")
{
    test_result original(888);
    test_result moved(std::move(original));

    REQUIRE(moved.has_value());
    REQUIRE(moved.value() == 888);
}

TEST_CASE("basic_result: move constructor preserves error state", "[basic_result]")
{
    test_result original(std::string("move error"));
    test_result moved(std::move(original));

    REQUIRE(moved.is_error());
    REQUIRE(moved.error() == "move error");
}

TEST_CASE("basic_result: copy assignment preserves value state", "[basic_result]")
{
    test_result src(111);
    test_result dst(std::string("old error"));

    dst = src;

    REQUIRE(dst.has_value());
    REQUIRE(dst.value() == 111);
}

TEST_CASE("basic_result: copy assignment preserves error state", "[basic_result]")
{
    test_result src(std::string("assigned error"));
    test_result dst(111);

    dst = src;

    REQUIRE(dst.is_error());
    REQUIRE(dst.error() == "assigned error");
}

TEST_CASE("basic_result: move assignment preserves value state", "[basic_result]")
{
    test_result src(222);
    test_result dst(std::string("old error"));

    dst = std::move(src);

    REQUIRE(dst.has_value());
    REQUIRE(dst.value() == 222);
}

TEST_CASE("basic_result: move assignment preserves error state", "[basic_result]")
{
    test_result src(std::string("moved assigned error"));
    test_result dst(222);

    dst = std::move(src);

    REQUIRE(dst.is_error());
    REQUIRE(dst.error() == "moved assigned error");
}