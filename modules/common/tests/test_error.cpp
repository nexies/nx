//
// Created by nexie on 4/1/2026.
//

#include <nx/common/types/errors/error.hpp>
#include <catch2/catch_test_macros.hpp>
#include <system_error>
#include <string>

TEST_CASE("error: default constructed is empty or neutral", "[nx::error]")
{
    nx::error err;

    REQUIRE_FALSE(static_cast<bool>(err));
    REQUIRE(err.value() == 0);
    REQUIRE_FALSE(err.commented());
    REQUIRE(err.comment().empty());
}

TEST_CASE("error: construct from errc", "[nx::error]")
{
    nx::error err(std::errc::invalid_argument);

    REQUIRE(static_cast<bool>(err));
    REQUIRE(err.value() != 0);
    REQUIRE(err.default_error_condition() == std::make_error_condition(std::errc::invalid_argument));
    REQUIRE_FALSE(err.description().empty());
    REQUIRE_FALSE(std::string(err.what()).empty());
}

TEST_CASE("error: construct from std::error_code", "[nx::error]")
{
    std::error_code ec = std::make_error_code(std::errc::permission_denied);
    nx::error err(ec);

    REQUIRE(static_cast<bool>(err));
    REQUIRE(err.value() == ec.value());
    REQUIRE(err.default_error_condition() == ec.default_error_condition());
    REQUIRE(err.category() == ec.category());
    REQUIRE_FALSE(err.description().empty());
}

TEST_CASE("error: construct from code and category", "[nx::error]")
{
    auto* cat = &std::generic_category();
    nx::error err(static_cast<int>(std::errc::timed_out), cat);

    REQUIRE(static_cast<bool>(err));
    REQUIRE(err.value() == static_cast<int>(std::errc::timed_out));
    REQUIRE(err.category() == *cat);
    REQUIRE_FALSE(err.description().empty());
}

TEST_CASE("error: construct with comment and location from code/category", "[nx::error]")
{
    auto* cat = &std::generic_category();
    nx::error err(
        static_cast<int>(std::errc::invalid_argument),
        cat,
        "bad parameter");

    REQUIRE(static_cast<bool>(err));
    REQUIRE(err.value() == static_cast<int>(std::errc::invalid_argument));
    REQUIRE(err.category() == *cat);
    REQUIRE(err.commented());
    REQUIRE(err.comment() == "bad parameter");
    REQUIRE(err.located());
    REQUIRE_FALSE(std::string(err.what()).empty());
}

TEST_CASE("error: construct with comment and location from std::error_code", "[nx::error]")
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

TEST_CASE("error: commented reflects comment presence", "[nx::error]")
{
    nx::error without_comment(std::errc::invalid_argument);
    nx::error with_comment(std::make_error_code(std::errc::invalid_argument), "custom message");

    REQUIRE_FALSE(without_comment.commented());
    REQUIRE(without_comment.comment().empty());

    REQUIRE(with_comment.commented());
    REQUIRE(with_comment.comment() == "custom message");
}

TEST_CASE("error: what is stable and non-empty for non-empty error", "[nx::error]")
{
    nx::error err(std::make_error_code(std::errc::no_such_file_or_directory), "file missing");

    const char* w1 = err.what();
    const char* w2 = err.what();

    REQUIRE(w1 != nullptr);
    REQUIRE(w2 != nullptr);
    REQUIRE_FALSE(std::string(w1).empty());
}

TEST_CASE("error: copy constructor preserves all visible fields", "[nx::error]")
{
    nx::error original(std::make_error_code(std::errc::operation_not_permitted), "copy me");
    nx::error copy(original);

    REQUIRE(static_cast<bool>(copy) == static_cast<bool>(original));
    REQUIRE(copy.value() == original.value());
    REQUIRE(copy.category() == original.category());
    REQUIRE(copy.default_error_condition() == original.default_error_condition());
    REQUIRE(copy.description() == original.description());
    REQUIRE(copy.comment() == original.comment());
    REQUIRE(copy.commented() == original.commented());
    REQUIRE(std::string(copy.what()) == std::string(original.what()));
    REQUIRE(copy.located() == original.located());
}

TEST_CASE("error: copy assignment preserves all visible fields", "[nx::error]")
{
    nx::error source(std::make_error_code(std::errc::broken_pipe), "copied comment");
    nx::error target;

    target = source;

    REQUIRE(static_cast<bool>(target) == static_cast<bool>(source));
    REQUIRE(target.value() == source.value());
    REQUIRE(target.category() == source.category());
    REQUIRE(target.default_error_condition() == source.default_error_condition());
    REQUIRE(target.description() == source.description());
    REQUIRE(target.comment() == source.comment());
    REQUIRE(target.commented() == source.commented());
    REQUIRE(std::string(target.what()) == std::string(source.what()));
    REQUIRE(target.located() == source.located());
}

TEST_CASE("error: move constructor preserves visible state in destination", "[nx::error]")
{
    nx::error source(std::make_error_code(std::errc::io_error), "move me");

    const int expected_value = source.value();
    const auto& expected_category = source.category();
    const auto expected_condition = source.default_error_condition();
    const std::string expected_description = source.description();
    const std::string expected_comment = source.comment();
    const bool expected_commented = source.commented();
    const bool expected_located = source.located();
    const std::string expected_what = source.what();

    nx::error moved(std::move(source));

    REQUIRE(static_cast<bool>(moved));
    REQUIRE(moved.value() == expected_value);
    REQUIRE(moved.category() == expected_category);
    REQUIRE(moved.default_error_condition() == expected_condition);
    REQUIRE(moved.description() == expected_description);
    REQUIRE(moved.comment() == expected_comment);
    REQUIRE(moved.commented() == expected_commented);
    REQUIRE(moved.located() == expected_located);
    REQUIRE(std::string(moved.what()) == expected_what);
}

TEST_CASE("error: move assignment preserves visible state in destination", "[nx::error]")
{
    nx::error source(std::make_error_code(std::errc::not_supported), "move assigned");
    nx::error target;

    const int expected_value = source.value();
    const auto& expected_category = source.category();
    const auto expected_condition = source.default_error_condition();
    const std::string expected_description = source.description();
    const std::string expected_comment = source.comment();
    const bool expected_commented = source.commented();
    const bool expected_located = source.located();
    const std::string expected_what = source.what();

    target = std::move(source);

    REQUIRE(static_cast<bool>(target));
    REQUIRE(target.value() == expected_value);
    REQUIRE(target.category() == expected_category);
    REQUIRE(target.default_error_condition() == expected_condition);
    REQUIRE(target.description() == expected_description);
    REQUIRE(target.comment() == expected_comment);
    REQUIRE(target.commented() == expected_commented);
    REQUIRE(target.located() == expected_located);
    REQUIRE(std::string(target.what()) == expected_what);
}

TEST_CASE("error: operator() creates derived error with same code and new comment", "[nx::error]")
{
    nx::error base(std::errc::invalid_argument);
    nx::error derived = base("extra explanation");

    REQUIRE(static_cast<bool>(derived));
    REQUIRE(derived.value() == base.value());
    REQUIRE(derived.category() == base.category());
    REQUIRE(derived.default_error_condition() == base.default_error_condition());

    REQUIRE(derived.commented());
    REQUIRE(derived.comment() == "extra explanation");
    REQUIRE(derived.located());
}

TEST_CASE("error: clear resets error to neutral state", "[nx::error]")
{
    nx::error err(std::make_error_code(std::errc::device_or_resource_busy), "must be cleared");

    REQUIRE(static_cast<bool>(err));
    REQUIRE(err.commented());

    err.clear();

    REQUIRE_FALSE(static_cast<bool>(err));
    REQUIRE(err.value() == 0);
    REQUIRE_FALSE(err.commented());
    REQUIRE(err.comment().empty());
}

TEST_CASE("error: location is present for commented factory/operator call", "[nx::error]")
{
    nx::error base(std::errc::invalid_argument);
    nx::error derived = base("comment with location");

    REQUIRE(derived.located());
}

TEST_CASE("error: description and what are usable after copy", "[nx::error]")
{
    nx::error original(std::make_error_code(std::errc::filename_too_long), "desc test");
    nx::error copy = original;

    REQUIRE_FALSE(copy.description().empty());
    REQUIRE_FALSE(std::string(copy.what()).empty());
}

TEST_CASE("error: category and default_error_condition stay consistent", "[nx::error]")
{
    std::error_code ec = std::make_error_code(std::errc::network_unreachable);
    nx::error err(ec, "network down");

    REQUIRE(err.category() == ec.category());
    REQUIRE(err.default_error_condition() == ec.default_error_condition());
}

TEST_CASE("error: equal default constructed errors compare equal", "[nx::error][compare]")
{
    nx::error lhs;
    nx::error rhs;

    REQUIRE(lhs == rhs);
    REQUIRE_FALSE(lhs != rhs);
}

TEST_CASE("error: errors with same errc compare equal", "[nx::error][compare]")
{
    nx::error lhs(std::errc::invalid_argument);
    nx::error rhs(std::errc::invalid_argument);

    REQUIRE(lhs == rhs);
    REQUIRE_FALSE(lhs != rhs);
}

TEST_CASE("error: errors with different values compare not equal", "[nx::error][compare]")
{
    nx::error lhs(std::errc::invalid_argument);
    nx::error rhs(std::errc::permission_denied);

    REQUIRE(lhs != rhs);
    REQUIRE_FALSE(lhs == rhs);
}

TEST_CASE("error: errors with same code but different comment compare not equal", "[nx::error][compare]")
{
    auto ec = std::make_error_code(std::errc::invalid_argument);

    nx::error lhs(ec, "first comment");
    nx::error rhs(ec, "second comment");

    REQUIRE(lhs == rhs);
    REQUIRE_FALSE(lhs != rhs);
    REQUIRE_FALSE(lhs.identical(rhs));
    REQUIRE(lhs.equivalent(rhs));
}

TEST_CASE("error: errors with same code and same comment compare equal", "[nx::error][compare]")
{
    auto ec = std::make_error_code(std::errc::invalid_argument);

    nx::error lhs(ec, "same comment");
    nx::error rhs(ec, "same comment");

    REQUIRE(lhs == rhs);
    REQUIRE_FALSE(lhs != rhs);
    REQUIRE_FALSE(lhs.identical(rhs));
    REQUIRE(lhs.equivalent(rhs));
}

TEST_CASE("error: copied error compares equal to original", "[nx::error][compare]")
{
    nx::error original(std::make_error_code(std::errc::io_error), "copyable");
    nx::error copy(original);

    REQUIRE(copy == original);
    REQUIRE_FALSE(copy != original);
    REQUIRE(original.identical(copy));
    REQUIRE(original.equivalent(copy));
}

TEST_CASE("error: assigned error compares equal to source", "[nx::error][compare]")
{
    nx::error source(std::make_error_code(std::errc::timed_out), "assigned");
    nx::error target;

    target = source;

    REQUIRE(target == source);
    REQUIRE_FALSE(target != source);
}

TEST_CASE("error: moved error destination compares equal to saved snapshot", "[nx::error][compare]")
{
    nx::error source(std::make_error_code(std::errc::broken_pipe), "moved");

    nx::error snapshot(source);
    nx::error moved(std::move(source));

    REQUIRE(moved == snapshot);
    REQUIRE_FALSE(moved != snapshot);
}

TEST_CASE("error: operator() with same comment produces equal errors", "[nx::error][compare]")
{
    nx::error base(std::errc::invalid_argument);

    nx::error lhs = base("same comment");
    nx::error rhs = base("same comment");

    REQUIRE(lhs == rhs);
    REQUIRE_FALSE(lhs != rhs);
    REQUIRE(lhs.equivalent(rhs));
    REQUIRE_FALSE(lhs.identical(rhs));
}

TEST_CASE("error: operator() with different comment produces different errors", "[nx::error][compare]")
{
    nx::error base(std::errc::invalid_argument);

    nx::error lhs = base("comment one");
    nx::error rhs = base("comment two");

    REQUIRE(lhs == rhs);
    REQUIRE_FALSE(lhs != rhs);
    REQUIRE(lhs.equivalent(rhs));
    REQUIRE_FALSE(lhs.identical(rhs));
}

TEST_CASE("error: cleared error no longer equals original non-empty error", "[nx::error][compare]")
{
    nx::error original(std::make_error_code(std::errc::device_or_resource_busy), "busy");
    nx::error cleared(original);

    cleared.clear();

    REQUIRE(cleared != original);
    REQUIRE_FALSE(cleared == original);
}