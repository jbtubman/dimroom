#pragma once

#include <algorithm>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "../include/utility.hpp"
#include "google_test_fixture.hpp"

namespace {
using std::pair;
using std::string;
using std::vector;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;
using std::operator""s;

struct utility_test_fixture : google_test_fixture {
    using pair_t = pair<int, char>;
    using pair_t_vec = vector<pair_t>;

    pair_t pair_1{1, 'a'};
    pair_t pair_2{2, 'b'};
    pair_t_vec pair_vec{pair_1, pair_2};

    const string trouble{"51.05011, -114.08529"};
    const string long_to_match{"-114.08529"};
    const string long_close_below{"-114.08528"};
    const string long_close_above{"-114.08530"};
    const string long_below_should_fail{"-114.08527"};
    const string long_above_should_fail{"-114.08531"};
};

}  // namespace

// Test is_close on float

TEST_F(utility_test_fixture, CloseFloatIsCloseSameFloatShouldWork) {
    const auto long_to_match_f = std::stof(utility_test_fixture::long_to_match);
    EXPECT_TRUE(is_close(long_to_match_f, long_to_match_f));
}

TEST_F(utility_test_fixture, CloseFloatIsCloseNearFloatBelowShouldWork) {
    const auto long_to_match_f = std::stof(utility_test_fixture::long_to_match);
    const auto long_close_below_f =
        std::stof(utility_test_fixture::long_close_below);
    EXPECT_TRUE(is_close(long_to_match_f, long_close_below_f));
}

TEST_F(utility_test_fixture, CloseFloatIsCloseNearFloatAboveShouldWork) {
    const auto long_to_match_f = std::stof(utility_test_fixture::long_to_match);
    const auto long_close_above_f =
        std::stof(utility_test_fixture::long_close_above);
    EXPECT_TRUE(is_close(long_to_match_f, long_close_above_f));
}

TEST_F(utility_test_fixture, CloseFloatIsCloseNotNearFloatBelowShouldFail) {
    const auto long_to_match_f = std::stof(utility_test_fixture::long_to_match);
    const auto long_below_should_fail_f =
        std::stof(utility_test_fixture::long_below_should_fail);
    EXPECT_FALSE(is_close(long_to_match_f, long_below_should_fail_f));
}

TEST_F(utility_test_fixture, CloseFloatIsCloseNotNearFloatAboveShouldFail) {
    const auto long_to_match_f = std::stof(utility_test_fixture::long_to_match);
    const auto llong_above_should_fail_f =
        std::stof(utility_test_fixture::long_above_should_fail);
    EXPECT_FALSE(is_close(long_to_match_f, llong_above_should_fail_f));
}

// Test is_close on double

TEST_F(utility_test_fixture, CloseDoubleIsCloseSameDoubleShouldWork) {
    const auto long_to_match_d = std::stod(utility_test_fixture::long_to_match);
    EXPECT_TRUE(is_close(long_to_match_d, long_to_match_d));
}

TEST_F(utility_test_fixture, CloseDoubleIsCloseNearDoubleBelowShouldWork) {
    const auto long_to_match_d = std::stod(utility_test_fixture::long_to_match);
    const auto long_close_below_d =
        std::stod(utility_test_fixture::long_close_below);
    EXPECT_TRUE(is_close(long_to_match_d, long_close_below_d));
}

TEST_F(utility_test_fixture, CloseDoubleIsCloseNearDoubleAboveShouldWork) {
    const auto long_to_match_d = std::stod(utility_test_fixture::long_to_match);
    const auto long_close_above_d =
        std::stod(utility_test_fixture::long_close_above);
    EXPECT_TRUE(is_close(long_to_match_d, long_close_above_d));
}

TEST_F(utility_test_fixture, CloseDoubleIsCloseNotNearDoubleBelowShouldFail) {
    const auto long_to_match_d = std::stod(utility_test_fixture::long_to_match);
    const auto long_below_should_fail_d =
        std::stod(utility_test_fixture::long_below_should_fail);
    EXPECT_FALSE(is_close(long_to_match_d, long_below_should_fail_d));
}

TEST_F(utility_test_fixture, CloseDoubleIsCloseNotNearDoubleAboveShouldFail) {
    const auto long_to_match_d = std::stod(utility_test_fixture::long_to_match);
    const auto long_above_should_fail_d =
        std::stod(utility_test_fixture::long_above_should_fail);
    EXPECT_FALSE(is_close(long_to_match_d, long_above_should_fail_d));
}

// Test is_close on long double

TEST_F(utility_test_fixture, CloseLongDoubleIsCloseSameLongDoubleShouldWork) {
    const auto long_to_match_ld =
        std::stold(utility_test_fixture::long_to_match);
    EXPECT_TRUE(is_close(long_to_match_ld, long_to_match_ld));
}

TEST_F(utility_test_fixture,
       CloseLongDoubleIsCloseNearLongDoubleBelowShouldWork) {
    const auto long_to_match_ld =
        std::stold(utility_test_fixture::long_to_match);
    const auto long_close_below_ld =
        std::stold(utility_test_fixture::long_close_below);
    EXPECT_TRUE(is_close(long_to_match_ld, long_close_below_ld));
}

TEST_F(utility_test_fixture,
       CloseLongDoubleIsCloseNearLongDoubleAboveShouldWork) {
    const auto long_to_match_ld =
        std::stold(utility_test_fixture::long_to_match);
    const auto long_close_above_ld =
        std::stold(utility_test_fixture::long_close_above);
    EXPECT_TRUE(is_close(long_to_match_ld, long_close_above_ld));
}

TEST_F(utility_test_fixture,
       CloseLongDoubleIsCloseNotNearLongDoubleBelowShouldFail) {
    const auto long_to_match_ld =
        std::stold(utility_test_fixture::long_to_match);
    const auto long_below_should_fail_ld =
        std::stold(utility_test_fixture::long_below_should_fail);
    EXPECT_FALSE(is_close(long_to_match_ld, long_below_should_fail_ld));
}

TEST_F(utility_test_fixture,
       CloseLongDoubleIsCloseNotNearLongDoubleAboveShouldFail) {
    const auto long_to_match_ld =
        std::stold(utility_test_fixture::long_to_match);
    const auto long_above_should_fail_ld =
        std::stold(utility_test_fixture::long_above_should_fail);
    EXPECT_FALSE(is_close(long_to_match_ld, long_above_should_fail_ld));
}

// Test string utilities.

TEST_F(utility_test_fixture, StringUtilsDequoteQuoted) {
    const string input = "\"foo\"";
    const string expected = "foo";
    const string result = dequote(input);
    EXPECT_TRUE(result == expected);
}

TEST_F(utility_test_fixture, StringUtilsDequoteUnquoted) {
    const string input = "foo";
    const string expected = "foo";
    const string result = dequote(input);
    EXPECT_TRUE(result == expected);
}

TEST_F(utility_test_fixture, StringUtilsDequoteQuotedInPlace) {
    string input = "\"foo\"";
    const string expected = "foo";
    dequote(input);
    EXPECT_TRUE(input == expected);
}

TEST_F(utility_test_fixture, StringUtilsDequoteUnquotedInPlace) {
    string input = "foo";
    const string expected = "foo";
    dequote(input);
    EXPECT_TRUE(input == expected);
}

TEST_F(utility_test_fixture, StringUtilsTrimWithSpace) {
    const string input = "  foo\r\n";
    const string expected = "  foo";
    const string result = trim(input);
    EXPECT_TRUE(result == expected);
}

TEST_F(utility_test_fixture, StringUtilsTrimWithNoSpace) {
    const string input = "foo\n";
    const string expected = "foo";
    const string result = trim(input);
    EXPECT_TRUE(result == expected);
}

TEST_F(utility_test_fixture, StringUtilsTrimWithSpaceInPlace) {
    string input = "  foo\r\n";
    const string expected = "  foo";
    trim(input);
    EXPECT_TRUE(input == expected);
}

TEST_F(utility_test_fixture, StringUtilsTrimWithNoSpaceInPlace) {
    string input = "foo\n";
    const string expected = "foo";
    trim(input);
    EXPECT_TRUE(input == expected);
}
