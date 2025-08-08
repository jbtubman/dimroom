#include "utility.hpp"

#include <algorithm>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "CppUnitTestFramework.hpp"
#include "../include/general_fixture.hpp"

namespace {
using std::pair;
using std::string;
using std::vector;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;
using std::operator""s;

struct MyFixture : general_fixture {
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

using std::pair;
using std::string;
using std::vector;

using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;

TEST_CASE(MyFixture, CloseFloat) {
    SECTION("is_close same float should work") {
        const auto long_to_match_f = std::stof(MyFixture::long_to_match);
        CHECK_TRUE(is_close(long_to_match_f, long_to_match_f));
    }

    SECTION("is_close near float below should work") {
        const auto long_to_match_f = std::stof(MyFixture::long_to_match);
        const auto long_close_below_f = std::stof(MyFixture::long_close_below);
        CHECK_TRUE(is_close(long_to_match_f, long_close_below_f));
    }

    SECTION("is_close near float above should work") {
        const auto long_to_match_f = std::stof(MyFixture::long_to_match);
        const auto long_close_above_f = std::stof(MyFixture::long_close_above);
        CHECK_TRUE(is_close(long_to_match_f, long_close_above_f));
    }

    SECTION("is_close not near float below should fail") {
        const auto long_to_match_f = std::stof(MyFixture::long_to_match);
        const auto long_below_should_fail_f =
            std::stof(MyFixture::long_below_should_fail);
        CHECK_FALSE(is_close(long_to_match_f, long_below_should_fail_f));
    }

    SECTION("is_close not near float above should fail") {
        const auto long_to_match_f = std::stof(MyFixture::long_to_match);
        const auto llong_above_should_fail_f =
            std::stof(MyFixture::long_above_should_fail);
        CHECK_FALSE(is_close(long_to_match_f, llong_above_should_fail_f));
    }
}

TEST_CASE(MyFixture, CloseDouble) {
    SECTION("is_close same double should work") {
        const auto long_to_match_d = std::stod(MyFixture::long_to_match);
        CHECK_TRUE(is_close(long_to_match_d, long_to_match_d));
    }

    SECTION("is_close near double below should work") {
        const auto long_to_match_d = std::stod(MyFixture::long_to_match);
        const auto long_close_below_d = std::stod(MyFixture::long_close_below);
        CHECK_TRUE(is_close(long_to_match_d, long_close_below_d));
    }

    SECTION("is_close near double above should work") {
        const auto long_to_match_d = std::stod(MyFixture::long_to_match);
        const auto long_close_above_d = std::stod(MyFixture::long_close_above);
        CHECK_TRUE(is_close(long_to_match_d, long_close_above_d));
    }

    SECTION("is_close not near double below should fail") {
        const auto long_to_match_d = std::stod(MyFixture::long_to_match);
        const auto long_below_should_fail_d =
            std::stod(MyFixture::long_below_should_fail);
        CHECK_FALSE(is_close(long_to_match_d, long_below_should_fail_d));
    }

    SECTION("is_close not near double above should fail") {
        const auto long_to_match_d = std::stod(MyFixture::long_to_match);
        const auto long_above_should_fail_d =
            std::stod(MyFixture::long_above_should_fail);
        CHECK_FALSE(is_close(long_to_match_d, long_above_should_fail_d));
    }
}

TEST_CASE(MyFixture, CloseLongDouble) {
    SECTION("is_close same long double should work") {
        const auto long_to_match_ld = std::stold(MyFixture::long_to_match);
        CHECK_TRUE(is_close(long_to_match_ld, long_to_match_ld));
    }

    SECTION("is_close near long double below should work") {
        const auto long_to_match_ld = std::stold(MyFixture::long_to_match);
        const auto long_close_below_ld =
            std::stold(MyFixture::long_close_below);
        CHECK_TRUE(is_close(long_to_match_ld, long_close_below_ld));
    }

    SECTION("is_close near long double above should work") {
        const auto long_to_match_ld = std::stold(MyFixture::long_to_match);
        const auto long_close_above_ld =
            std::stold(MyFixture::long_close_above);
        CHECK_TRUE(is_close(long_to_match_ld, long_close_above_ld));
    }

    SECTION("is_close not near long double below should fail") {
        const auto long_to_match_ld = std::stold(MyFixture::long_to_match);
        const auto long_below_should_fail_ld =
            std::stold(MyFixture::long_below_should_fail);
        CHECK_FALSE(is_close(long_to_match_ld, long_below_should_fail_ld));
    }

    SECTION("is_close not near long double above should fail") {
        const auto long_to_match_ld = std::stold(MyFixture::long_to_match);
        const auto long_above_should_fail_ld =
            std::stold(MyFixture::long_above_should_fail);
        CHECK_FALSE(is_close(long_to_match_ld, long_above_should_fail_ld));
    }
}

TEST_CASE(MyFixture, StringUtils) {
    SECTION("dequote quoted") {
        const string input = "\"foo\"";
        const string expected = "foo";
        const string result = dequote(input);
        CHECK_TRUE(result == expected);
    }

    SECTION("dequote unquoted") {
        const string input = "foo";
        const string expected = "foo";
        const string result = dequote(input);
        CHECK_TRUE(result == expected);
    }
    SECTION("dequote quoted in place") {
        string input = "\"foo\"";
        const string expected = "foo";
        dequote(input);
        CHECK_TRUE(input == expected);
    }

    SECTION("dequote unquoted in place") {
        string input = "foo";
        const string expected = "foo";
        dequote(input);
        CHECK_TRUE(input == expected);
    }

    SECTION("trim with space") {
        const string input = "  foo\r\n";
        const string expected = "  foo";
        const string result = trim(input);
        CHECK_TRUE(result == expected);
    }

    SECTION("trim with no space") {
        const string input = "foo\n";
        const string expected = "foo";
        const string result = trim(input);
        CHECK_TRUE(result == expected);
    }

    SECTION("trim with space in place") {
        string input = "  foo\r\n";
        const string expected = "  foo";
        trim(input);
        CHECK_TRUE(input == expected);
    }

    SECTION("trim with no space in place") {
        string input = "foo\n";
        const string expected = "foo";
        trim(input);
        CHECK_TRUE(input == expected);
    }
}
