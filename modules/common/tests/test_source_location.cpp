//
// Created by nexie on 30.03.2026.
//


#include <catch2/catch_test_macros.hpp>
#include <nx/common/types/source_location.hpp>

#include <sstream>
#include <string>
#include <filesystem>

TEST_CASE("source_location: explicit construction stores fields", "[nx::source_location]")
{
    nx::source_location loc(10, 20, "test.cpp", "test_func");

    REQUIRE(loc.line() == 10);

    if constexpr (nx::source_location::has_builtin_column)
        REQUIRE(loc.column() == 20);
    else
        REQUIRE(loc.column() == 0);

    REQUIRE(loc.file() == "test.cpp");
    REQUIRE(loc.function() == "test_func");
}

TEST_CASE("source_location: filepath returns filesystem path", "[nx::source_location]")
{
    nx::source_location loc(1, 2, "dir/subdir/file.cpp", "func");

    REQUIRE(loc.filepath() == std::filesystem::path("dir/subdir/file.cpp"));
    REQUIRE(loc.filepath().filename() == std::filesystem::path("file.cpp"));
}

TEST_CASE("source_location: current captures call site information", "[nx::source_location]")
{
    auto loc = nx::source_location::current();

    REQUIRE(loc.line() > 0);
    REQUIRE_FALSE(loc.file().empty());
    REQUIRE_FALSE(loc.function().empty());
}

TEST_CASE("source_location: current preserves explicitly passed location", "[nx::source_location]")
{
    nx::source_location src(123, 7, "sample.cpp", "sample_func");

    auto loc = nx::source_location::current(src);

    REQUIRE(loc == src);
    REQUIRE_FALSE(loc != src);
}

TEST_CASE("source_location: short_link contains filename and line", "[nx::source_location]")
{
    nx::source_location loc(42, 3, "/tmp/project/test_file.cpp", "func");

    auto short_link = loc.short_link();

    REQUIRE(short_link.find("test_file.cpp:42") != std::string::npos);

    if constexpr (nx::source_location::has_builtin_column)
        REQUIRE(short_link.find(":3") != std::string::npos);
}

TEST_CASE("source_location: description contains main fields", "[nx::source_location]")
{
    nx::source_location loc(55, 8, "/home/user/source.cpp", "my_function");

    auto desc = loc.description();

    REQUIRE(desc.find("Source Location:") != std::string::npos);
    REQUIRE(desc.find("source.cpp:55") != std::string::npos);
    REQUIRE(desc.find("Function: my_function") != std::string::npos);
    REQUIRE(desc.find("File: /home/user/source.cpp") != std::string::npos);
    REQUIRE(desc.find("Line: 55") != std::string::npos);

    if constexpr (nx::source_location::has_builtin_column)
        REQUIRE(desc.find("Column: 8") != std::string::npos);
}

TEST_CASE("source_location: description omits function line when function is empty", "[nx::source_location]")
{
    nx::source_location loc(12, 0, "plain.cpp", "");

    auto desc = loc.description();

    REQUIRE(desc.find("Source Location:") != std::string::npos);
    REQUIRE(desc.find("plain.cpp:12") != std::string::npos);
    REQUIRE(desc.find("File: plain.cpp") != std::string::npos);
    REQUIRE(desc.find("Line: 12") != std::string::npos);
    REQUIRE(desc.find("Function:") == std::string::npos);
}

TEST_CASE("source_location: equality compares all fields", "[nx::source_location]")
{
    nx::source_location lhs(10, 2, "a.cpp", "func");
    nx::source_location rhs(10, 2, "a.cpp", "func");

    REQUIRE(lhs == rhs);
    REQUIRE_FALSE(lhs != rhs);
}

TEST_CASE("source_location: inequality detects different line", "[nx::source_location]")
{
    nx::source_location lhs(10, 2, "a.cpp", "func");
    nx::source_location rhs(11, 2, "a.cpp", "func");

    REQUIRE(lhs != rhs);
    REQUIRE_FALSE(lhs == rhs);
}

TEST_CASE("source_location: inequality detects different column", "[nx::source_location]")
{
    nx::source_location lhs(10, 2, "a.cpp", "func");
    nx::source_location rhs(10, 3, "a.cpp", "func");

    REQUIRE(lhs != rhs);
    REQUIRE_FALSE(lhs == rhs);
}

TEST_CASE("source_location: inequality detects different file", "[nx::source_location]")
{
    nx::source_location lhs(10, 2, "a.cpp", "func");
    nx::source_location rhs(10, 2, "b.cpp", "func");

    REQUIRE(lhs != rhs);
    REQUIRE_FALSE(lhs == rhs);
}

TEST_CASE("source_location: inequality detects different function", "[nx::source_location]")
{
    nx::source_location lhs(10, 2, "a.cpp", "func1");
    nx::source_location rhs(10, 2, "a.cpp", "func2");

    REQUIRE(lhs != rhs);
    REQUIRE_FALSE(lhs == rhs);
}

TEST_CASE("source_location: stream output matches description", "[nx::source_location]")
{
    nx::source_location loc(77, 1, "stream.cpp", "stream_func");

    std::ostringstream os;
    os << loc;

    REQUIRE(os.str() == loc.description());
}

TEST_CASE("source_location: undefined location has expected sentinel values", "[nx::source_location]")
{
    REQUIRE(nx::g_undefined_location.line() == 0);
    REQUIRE(nx::g_undefined_location.column() == 0);
    REQUIRE(nx::g_undefined_location.file() == "undefined");
    REQUIRE(nx::g_undefined_location.function() == "undefined");
}

TEST_CASE("source_location: undefined location compares equal to itself", "[nx::source_location]")
{
    REQUIRE(nx::g_undefined_location == nx::g_undefined_location);
    REQUIRE_FALSE(nx::g_undefined_location != nx::g_undefined_location);
}

TEST_CASE("source_location: copied object remains equal", "[nx::source_location]")
{
    nx::source_location original(9, 4, "copy.cpp", "copy_func");
    nx::source_location copy(original);

    REQUIRE(copy == original);
    REQUIRE(copy.short_link() == original.short_link());
    REQUIRE(copy.description() == original.description());
}