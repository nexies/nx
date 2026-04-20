//
// Created by nexie on 4/1/2026.
//

#include <nx/common/types/errors.hpp>
#include <catch2/catch_test_macros.hpp>
#include <system_error>
#include <string>

// ── nx::error base ───────────────────────────────────────────────────────────

TEST_CASE("error: default constructed is neutral", "[nx::error]")
{
    nx::error err;
    REQUIRE_FALSE(static_cast<bool>(err));
    REQUIRE(err.value() == 0);
    REQUIRE_FALSE(err.commented());
    REQUIRE(err.comment().empty());
}

TEST_CASE("error: construct from message (code=1)", "[nx::error]")
{
    nx::error err("something went wrong");
    REQUIRE(static_cast<bool>(err));
    REQUIRE(err.value() == 1);
    REQUIRE(err.commented());
    REQUIRE(err.comment() == "something went wrong");
    REQUIRE(err.located());
    REQUIRE_FALSE(std::string(err.what()).empty());
}

TEST_CASE("error: construct from code and message", "[nx::error]")
{
    nx::error err(42, "custom error");
    REQUIRE(static_cast<bool>(err));
    REQUIRE(err.value() == 42);
    REQUIRE(err.comment() == "custom error");
    REQUIRE(err.located());
}

TEST_CASE("error: construct from std::error_code", "[nx::error]")
{
    std::error_code ec = std::make_error_code(std::errc::permission_denied);
    nx::error err(ec);

    REQUIRE(static_cast<bool>(err));
    REQUIRE(err.value() == ec.value());
    REQUIRE(err.category() == ec.category());
    REQUIRE_FALSE(err.description().empty());
    REQUIRE_FALSE(std::string(err.what()).empty());
}

TEST_CASE("error: construct from std::error_code with comment", "[nx::error]")
{
    std::error_code ec = std::make_error_code(std::errc::address_in_use);
    nx::error err(ec, "socket bind failed");

    REQUIRE(static_cast<bool>(err));
    REQUIRE(err.value() == ec.value());
    REQUIRE(err.category() == ec.category());
    REQUIRE(err.commented());
    REQUIRE(err.comment() == "socket bind failed");
    REQUIRE(err.located());
    REQUIRE_FALSE(std::string(err.what()).empty());
}

TEST_CASE("error: nx-native description is empty", "[nx::error]")
{
    nx::error err("native message");
    REQUIRE(err.description().empty());
    REQUIRE(err.commented());
}

TEST_CASE("error: std::error_code description is non-empty", "[nx::error]")
{
    nx::error err(std::make_error_code(std::errc::no_such_file_or_directory));
    REQUIRE_FALSE(err.description().empty());
}

TEST_CASE("error: what() includes both description and comment for error_code path", "[nx::error]")
{
    auto ec = std::make_error_code(std::errc::permission_denied);
    nx::error err(ec, "cannot open /etc/shadow");
    std::string w = err.what();
    REQUIRE(((w.find("permission") != std::string::npos) || (w.find("Permission") != std::string::npos)));
    REQUIRE(w.find("cannot open") != std::string::npos);
}

TEST_CASE("error: what() shows comment for nx-native error", "[nx::error]")
{
    nx::error err("file not found");
    std::string w = err.what();
    REQUIRE(w.find("file not found") != std::string::npos);
}

TEST_CASE("error: copy constructor preserves all visible fields", "[nx::error]")
{
    nx::error original("copy me");
    nx::error copy(original);

    REQUIRE(static_cast<bool>(copy) == static_cast<bool>(original));
    REQUIRE(copy.value()       == original.value());
    REQUIRE(copy.category()    == original.category());
    REQUIRE(copy.description() == original.description());
    REQUIRE(copy.comment()     == original.comment());
    REQUIRE(copy.commented()   == original.commented());
    REQUIRE(copy.located()     == original.located());
    REQUIRE(std::string(copy.what()) == std::string(original.what()));
}

TEST_CASE("error: copy assignment preserves all visible fields", "[nx::error]")
{
    nx::error source(std::make_error_code(std::errc::broken_pipe), "copied comment");
    nx::error target;
    target = source;

    REQUIRE(target.value()     == source.value());
    REQUIRE(target.category()  == source.category());
    REQUIRE(target.comment()   == source.comment());
    REQUIRE(target.commented() == source.commented());
    REQUIRE(target.located()   == source.located());
}

TEST_CASE("error: move constructor transfers state", "[nx::error]")
{
    nx::error source("move me");
    const int expected_value   = source.value();
    const std::string expected = source.comment();

    nx::error moved(std::move(source));

    REQUIRE(static_cast<bool>(moved));
    REQUIRE(moved.value()   == expected_value);
    REQUIRE(moved.comment() == expected);
    REQUIRE(moved.located());
}

TEST_CASE("error: move assignment transfers state", "[nx::error]")
{
    nx::error source(std::make_error_code(std::errc::not_supported), "move assigned");
    const int expected_value = source.value();

    nx::error target;
    target = std::move(source);

    REQUIRE(static_cast<bool>(target));
    REQUIRE(target.value() == expected_value);
}

TEST_CASE("error: operator() creates copy with new comment", "[nx::error]")
{
    nx::error base("base error");
    nx::error derived = base("extra context");

    REQUIRE(derived.value()    == base.value());
    REQUIRE(derived.category() == base.category());
    REQUIRE(derived.comment()  == "extra context");
    REQUIRE(derived.located());
}

TEST_CASE("error: clear resets to neutral state", "[nx::error]")
{
    nx::error err("must be cleared");
    REQUIRE(static_cast<bool>(err));

    err.clear();

    REQUIRE_FALSE(static_cast<bool>(err));
    REQUIRE(err.value() == 0);
    REQUIRE_FALSE(err.commented());
}

// ── comparisons ──────────────────────────────────────────────────────────────

TEST_CASE("error: two default errors are equal", "[nx::error][compare]")
{
    nx::error lhs, rhs;
    REQUIRE(lhs == rhs);
    REQUIRE_FALSE(lhs != rhs);
}

TEST_CASE("error: same message errors are equal (same code+category)", "[nx::error][compare]")
{
    nx::error lhs("foo");
    nx::error rhs("foo");
    REQUIRE(lhs == rhs);
}

TEST_CASE("error: same category different code are not equal", "[nx::error][compare]")
{
    nx::error lhs(1, "a");
    nx::error rhs(2, "b");
    REQUIRE(lhs != rhs);
}

TEST_CASE("error: identical requires same location", "[nx::error][compare]")
{
    auto ec = std::make_error_code(std::errc::invalid_argument);
    nx::error a(ec, "msg");
    nx::error b(ec, "msg");
    REQUIRE(a == b);
    REQUIRE_FALSE(a.identical(b));  // different locations
}

TEST_CASE("error: copy compares identical to original", "[nx::error][compare]")
{
    nx::error original(std::make_error_code(std::errc::io_error), "copyable");
    nx::error copy(original);
    REQUIRE(copy == original);
    REQUIRE(original.identical(copy));
}

TEST_CASE("error: cleared error != original", "[nx::error][compare]")
{
    nx::error original("busy");
    nx::error cleared(original);
    cleared.clear();
    REQUIRE(cleared != original);
}

// ── class hierarchy ──────────────────────────────────────────────────────────

TEST_CASE("runtime_error: basic construction", "[nx::runtime_error]")
{
    nx::err::runtime_error err("network failure");
    REQUIRE(static_cast<bool>(err));
    REQUIRE(err.value() == 1);
    REQUIRE(err.comment() == "network failure");
    REQUIRE(err.located());
    std::string w = err.what();
    REQUIRE(w.find("runtime") != std::string::npos);
    REQUIRE(w.find("network failure") != std::string::npos);
}

TEST_CASE("runtime_error: with explicit code", "[nx::runtime_error]")
{
    nx::err::runtime_error err(5, "retry exceeded");
    REQUIRE(err.value() == 5);
    REQUIRE(err.comment() == "retry exceeded");
}

TEST_CASE("logic_error: basic construction", "[nx::logic_error]")
{
    nx::err::logic_error err("precondition violated");
    REQUIRE(static_cast<bool>(err));
    std::string w = err.what();
    REQUIRE(w.find("logic") != std::string::npos);
}

TEST_CASE("access_error: inherits runtime_error", "[nx::access_error]")
{
    nx::err::access_error err("permission denied");
    REQUIRE(static_cast<bool>(err));
    REQUIRE(err.comment() == "permission denied");

    // must be catchable as runtime_error and as error
    nx::err::runtime_error& as_runtime = err;
    nx::error&         as_error   = err;
    REQUIRE(static_cast<bool>(as_runtime));
    REQUIRE(static_cast<bool>(as_error));
}

TEST_CASE("invalid_argument: inherits logic_error", "[nx::invalid_argument]")
{
    nx::err::invalid_argument err("count must be positive");
    REQUIRE(static_cast<bool>(err));

    nx::err::logic_error& as_logic = err;
    nx::error&       as_error = err;
    REQUIRE(static_cast<bool>(as_logic));
    REQUIRE(static_cast<bool>(as_error));
}

TEST_CASE("error types with same code but different categories are not equal", "[nx::error][compare]")
{
    nx::err::runtime_error rt("x");
    nx::err::logic_error   lc("x");
    nx::error         base("x");

    REQUIRE(rt != lc);
    REQUIRE(rt != base);
    REQUIRE(lc != base);
}

TEST_CASE("access_error != runtime_error even with same code", "[nx::error][compare]")
{
    nx::err::access_error  ae("denied");
    nx::err::runtime_error re("denied");
    REQUIRE(ae != re);
}

TEST_CASE("memory_error: basic construction", "[nx::memory_error]")
{
    nx::err::memory_error err("out of pool space");
    REQUIRE(static_cast<bool>(err));
    std::string w = err.what();
    REQUIRE(w.find("memory") != std::string::npos);
}

TEST_CASE("not_supported: inherits logic_error", "[nx::not_supported]")
{
    nx::err::not_supported err("feature X is not implemented");
    nx::err::logic_error& as_logic = err;
    REQUIRE(static_cast<bool>(as_logic));
}

TEST_CASE("invalid_state: inherits logic_error", "[nx::invalid_state]")
{
    nx::err::invalid_state err("cannot call start() twice");
    nx::err::logic_error& as_logic = err;
    REQUIRE(static_cast<bool>(as_logic));
}

TEST_CASE("error hierarchy catchable via C++ exception handling", "[nx::error]")
{
    auto throw_access = [] { throw nx::err::access_error("forbidden"); };

    bool caught_runtime = false;
    bool caught_base    = false;

    try { throw_access(); }
    catch (const nx::err::runtime_error&) { caught_runtime = true; }
    catch (...) {}

    try { throw_access(); }
    catch (const nx::error&) { caught_base = true; }
    catch (...) {}

    REQUIRE(caught_runtime);
    REQUIRE(caught_base);
}
