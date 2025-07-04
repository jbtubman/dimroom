#include <algorithm>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

#include "CppUnitTestFramework.hpp"
#include "../parser.hpp"
#include "../parse_utils.hpp"
#include "general_fixture.hpp"

namespace {
using std::string;
using std::vector;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;

struct MyFixture : general_fixture {
    //...

    // TODO: move column-related stuff to the column tests.
    const vector<column> sample_cols = {column("Filename", 0),
                                        column("Type", 1),
                                        column("Image Size (MB)", 2),
                                        column("Image X", 3),
                                        column("Image Y", 4),
                                        column("DPI", 5),
                                        column("(Center) Coordinate", 6),
                                        column("Favorite", 7),
                                        column("Continent", 8),
                                        column("Bit color", 9),
                                        column("Alpha", 10),
                                        column("Hockey Team", 11),
                                        column("User Tags", 12)};
    using cols_view_t = decltype(views::all(sample_cols));
    cols_view_t sample_cols_view = views::all(sample_cols);
    const columns sample_columns{sample_cols_view};

};
}  // namespace

using std::string;
using std::vector;

using std::println;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;

TEST_CASE(MyFixture, ParseHeader) {
    SECTION("parse sample header") {
        using std::operator""sv;

        auto parsed_columns = parse_header(MyFixture::sample_header);

        CHECK_TRUE(parsed_columns.mostly_equal(MyFixture::sample_columns));
    }

    SECTION("find triple-quoted fields") {}
}

TEST_CASE(MyFixture, ParseRow) {
  SECTION("row value types") {
    auto parsed_strings = fix_quoted_fields(MyFixture::sample_row_3);
    auto val_types = row_value_types(parsed_strings);
    int i = 0;
    for (auto v : val_types) {
      println(stderr, "row value types - val_types[{}]: {}", i++, str(v));
    }
  }
}
