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
#include "general_fixture.hpp"

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

    using flipped_pair_t = flip_pair_type_t<pair_t>;
    using flipped_pair_t_vec = vector<flipped_pair_t>;

    pair_t pair_1{1, 'a'};
    pair_t pair_2{2, 'b'};
    pair_t_vec pair_vec{pair_1, pair_2};
};

}  // namespace

using std::pair;
using std::string;
using std::vector;

using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;

TEST_CASE(MyFixture, Flip) {
    SECTION("flip_pair") {
        const auto input = pair<int, char>(1, 'a');
        const auto expected = flipped_pair_t{'a', 1};
        // const auto result = flip_pair(std::pair<int,char>(1, 'a'));
        const auto result = flip_pair(input);
        CHECK_TRUE(result == expected);
    }

    SECTION("flip_pairs") {
        const auto input = MyFixture::pair_vec;
        const MyFixture::flipped_pair_t_vec expected{{'a', 1}, {'b', 2}};
        const auto result = flip_pairs(input);
        CHECK_TRUE(result == expected);
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
