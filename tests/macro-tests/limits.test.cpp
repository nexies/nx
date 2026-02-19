//
// Created by nexie on 19.02.2026.
//

#include <catch2/catch_test_macros.hpp>
#include <nx/macro.hpp>
#include <nx/macro/detail/arg_tk_limit.hpp>
#include <nx/macro/detail/bool_limit.hpp>
#include <nx/macro/detail/choose_limit.hpp>
#include <nx/macro/detail/is_max_limit.hpp>
#include <nx/macro/detail/number_dec_limit.hpp>
#include <nx/macro/detail/number_inc_limit.hpp>
#include <nx/macro/detail/sequence_limit.hpp>
#include <nx/macro/detail/while_limit.hpp>
#include <nx/macro/detail/put_at_limit.hpp>


TEST_CASE("Macro Numeric Limits", "")
{
    REQUIRE(NX_LIMITS_BOOL >= NX_LIMITS_MAX_NUMBER);
    REQUIRE(NX_LIMITS_ARG_TK >= NX_LIMITS_MAX_NUMBER);
    REQUIRE(NX_LIMITS_CHOOSE >= NX_LIMITS_MAX_NUMBER);
    REQUIRE(NX_LIMITS_IS_MAX >= NX_LIMITS_MAX_NUMBER);
    REQUIRE(NX_LIMITS_SEQUENCE >= NX_LIMITS_MAX_NUMBER);
    REQUIRE(NX_LIMITS_INC >= NX_LIMITS_MAX_NUMBER);
    REQUIRE(NX_LIMITS_DEC >= NX_LIMITS_MAX_NUMBER);
    REQUIRE(NX_LIMITS_WHILE >= NX_LIMITS_MAX_NUMBER);
    REQUIRE(NX_LIMITS_PUT_AT >= NX_LIMITS_MAX_NUMBER);
}

// TEST_CASE("Macro ARG_TK numeric limit", "")
// {
// }
//
// TEST_CASE("Macro CHOOSE numeric limit", "")
// {
// }
//
// TEST_CASE("Macro IS_MAX numeric limit", "")
// {
// }
//
// TEST_CASE("Macro SEQUENCE numeric limit", "")
// {
// }
//
// TEST_CASE("Macro INC numeric limit", "")
// {
// }
//
// TEST_CASE("Macro DEC numeric limit", "")
// {
// }