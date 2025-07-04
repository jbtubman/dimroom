#include "../parser.hpp"

#include <algorithm>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

#include "../parse_utils.hpp"
#include "CppUnitTestFramework.hpp"
#include "general_fixture.hpp"
#include "../parser_formatter.hpp"

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

// TODO: proper tests for parser_test.cpp
TEST_CASE(MyFixture, ParseHeader) {
    SECTION("parse sample header") {
        parser::header_fields result =
            parser::parse_header(MyFixture::sample_header);
        vector<string> expected = MyFixture::sample_header_fields;

        CHECK_TRUE(result.size() == expected.size());
        if (result.size() != expected.size()) {
            println(stderr, "result.size() {} != expected.size() {}", result.size(), expected.size());
            println(stderr, "result: {}", result);
        }
        println(stderr, "result: {}", result);

        CHECK_TRUE(ranges::all_of(
            ranges::zip_view(result, expected),
            [](auto r_e_pair) {
                return r_e_pair.first.name == r_e_pair.second;
            }));

        CHECK_TRUE(ranges::all_of(result, [](auto h) {
            return h.data_type == e_cell_data_type::undetermined;
        }));
    }

    SECTION("parse sample header as columns") {
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
