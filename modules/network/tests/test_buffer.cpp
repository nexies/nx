#include <nx/network/buffer.hpp>
#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>

using nx::network::read_buffer;

// ── Helpers ───────────────────────────────────────────────────────────────────

static void push(read_buffer & b, std::string_view s)
{
    auto sp = b.prepare(s.size());
    std::memcpy(sp.data(), s.data(), s.size());
    b.commit(s.size());
}

static std::string peek(const read_buffer & b)
{
    auto d = b.data();
    return { d.data(), d.size() };
}

// ── Construction ──────────────────────────────────────────────────────────────

TEST_CASE("read_buffer: default constructed is empty", "[buffer]")
{
    read_buffer b;
    REQUIRE(b.empty());
    REQUIRE(b.size() == 0);
    REQUIRE(b.data().empty());
}

// ── Write then read ───────────────────────────────────────────────────────────

TEST_CASE("read_buffer: commit/data round-trip", "[buffer]")
{
    read_buffer b;
    push(b, "hello");

    REQUIRE(b.size() == 5);
    REQUIRE(peek(b) == "hello");
}

TEST_CASE("read_buffer: consume advances read pointer", "[buffer]")
{
    read_buffer b;
    push(b, "abcde");

    b.consume(2);
    REQUIRE(b.size() == 3);
    REQUIRE(peek(b) == "cde");
}

TEST_CASE("read_buffer: consuming all resets to empty", "[buffer]")
{
    read_buffer b;
    push(b, "xyz");
    b.consume(3);

    REQUIRE(b.empty());
}

TEST_CASE("read_buffer: multiple pushes accumulate", "[buffer]")
{
    read_buffer b;
    push(b, "foo");
    push(b, "bar");

    REQUIRE(b.size() == 6);
    REQUIRE(peek(b) == "foobar");
}

TEST_CASE("read_buffer: partial consume then more data", "[buffer]")
{
    read_buffer b;
    push(b, "hello world");
    b.consume(6);        // "world" remaining
    push(b, "!");

    REQUIRE(peek(b) == "world!");
}

// ── try_read_exactly ──────────────────────────────────────────────────────────

TEST_CASE("read_buffer: try_read_exactly returns nullopt when not enough data", "[buffer]")
{
    read_buffer b;
    push(b, "hi");

    REQUIRE_FALSE(b.try_read_exactly(5).has_value());
}

TEST_CASE("read_buffer: try_read_exactly returns view when enough data", "[buffer]")
{
    read_buffer b;
    push(b, "hello");

    auto r = b.try_read_exactly(3);
    REQUIRE(r.has_value());
    REQUIRE(std::string(r->data(), r->size()) == "hel");
    REQUIRE(b.size() == 5);  // not consumed
}

// ── try_read_until(char) ──────────────────────────────────────────────────────

TEST_CASE("read_buffer: try_read_until returns nullopt when delim absent", "[buffer]")
{
    read_buffer b;
    push(b, "no newline here");

    REQUIRE_FALSE(b.try_read_until('\n').has_value());
}

TEST_CASE("read_buffer: try_read_until includes delimiter", "[buffer]")
{
    read_buffer b;
    push(b, "line1\nrest");

    auto r = b.try_read_until('\n');
    REQUIRE(r.has_value());
    REQUIRE(std::string(r->data(), r->size()) == "line1\n");
    REQUIRE(b.size() == 10);  // not consumed
}

TEST_CASE("read_buffer: try_read_until delimiter at position 0", "[buffer]")
{
    read_buffer b;
    push(b, "\nstuff");

    auto r = b.try_read_until('\n');
    REQUIRE(r.has_value());
    REQUIRE(r->size() == 1);
}

// ── try_read_until(string_view) ───────────────────────────────────────────────

TEST_CASE("read_buffer: try_read_until string_view", "[buffer]")
{
    read_buffer b;
    push(b, "GET / HTTP/1.1\r\n\r\n");

    auto r = b.try_read_until("\r\n");
    REQUIRE(r.has_value());
    REQUIRE(std::string(r->data(), r->size()) == "GET / HTTP/1.1\r\n");
}

TEST_CASE("read_buffer: try_read_until string_view not found", "[buffer]")
{
    read_buffer b;
    push(b, "incomplete");

    REQUIRE_FALSE(b.try_read_until("\r\n").has_value());
}

// ── Capacity / compaction / growth ────────────────────────────────────────────

TEST_CASE("read_buffer: clears all data", "[buffer]")
{
    read_buffer b;
    push(b, "data");
    b.clear();

    REQUIRE(b.empty());
}

TEST_CASE("read_buffer: grows when capacity exceeded", "[buffer]")
{
    read_buffer b(8);  // small capacity

    const std::string big(100, 'x');
    push(b, big);

    REQUIRE(b.size() == 100);
    REQUIRE(b.capacity() >= 100);
    REQUIRE(peek(b) == big);
}

TEST_CASE("read_buffer: compacts when head passes midpoint", "[buffer]")
{
    read_buffer b(16);
    push(b, "0123456789abcdef");   // fill completely
    b.consume(9);                   // head_ = 9 > 16/2, triggers compact on next consume
    b.consume(1);                   // head_ = 10 > 8 → compact

    REQUIRE(peek(b) == "abcdef");
    // After compact, head_ == 0 so more data fits without realloc
    push(b, "XY");
    REQUIRE(peek(b) == "abcdefXY");
}
