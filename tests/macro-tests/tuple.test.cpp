//
// Created by nexie on 17.02.2026.
//

#include <catch2/catch_test_macros.hpp>
#include <nx/macro/tuple/tuple.hpp>

TEST_CASE("Macro Tuple Get", "_nx_tuple_get")
{
    REQUIRE(_nx_tuple_get(3, _nx_tuple(1, 2, 3, 4, 5)) == 4);
    REQUIRE(_nx_tuple_get(0, _nx_tuple(0, 1, 2, 3, 4)) == 0);
}

TEST_CASE("Macro Tuple Empty", "_nx_tuple_empty")
{
    REQUIRE_FALSE(_nx_tuple_empty(_nx_tuple(1, 2, 3, 4)));
    REQUIRE(_nx_tuple_empty(_nx_tuple()));

    REQUIRE_FALSE(_nx_tuple_not_empty(_nx_tuple()));
    REQUIRE(_nx_tuple_not_empty(_nx_tuple(1, 2, 3, 4, 5)));
}

#define REQUIRE_TUPLE_EQUAL(t1, t2) REQUIRE(std::string_view(NX_TO_STRING(t1)) == std::string_view(NX_TO_STRING(t2)))

TEST_CASE("Macro Tuple Append", "_nx_tuple_append")
{
    // simple append
    REQUIRE_TUPLE_EQUAL(_nx_tuple_append(_nx_tuple(1, 2, 3), 4), _nx_tuple(1, 2, 3, 4));

    // append with on arguments
    REQUIRE_TUPLE_EQUAL(_nx_tuple_append(_nx_tuple(1, 2, 3)), _nx_tuple(1, 2, 3));

    // append with empty tuple
    REQUIRE_TUPLE_EQUAL(_nx_tuple_append(_nx_tuple(), 1, 2, 3), _nx_tuple(1, 2, 3));
}

TEST_CASE("Macro Tuple Concat", "_nx_tuple_concat")
{
    // simple concatenation
    REQUIRE_TUPLE_EQUAL(_nx_tuple_concat(_nx_tuple(1, 2, 3), _nx_tuple(5, 6, 7)), _nx_tuple(1, 2, 3, 5, 6, 7));

    // concatenation with empty second tuple
    REQUIRE_TUPLE_EQUAL(_nx_tuple_concat(_nx_tuple(1, 2, 3), _nx_tuple()), _nx_tuple(1, 2, 3));

    // concatenation with empty first tuple
    REQUIRE_TUPLE_EQUAL(_nx_tuple_concat(_nx_tuple(), _nx_tuple(1, 2, 3)), _nx_tuple(1, 2, 3));
}