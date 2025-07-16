#include "utility.hpp"

#include <algorithm>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>
#include <utility>

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

using std::string;
using std::vector;
using std::pair;

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
