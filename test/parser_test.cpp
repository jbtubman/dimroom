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

#include "../cell_types.hpp"
#include "../parse_utils.hpp"
#include "../parser_formatter.hpp"
#include "CppUnitTestFramework.hpp"
#include "general_fixture.hpp"

namespace {
using std::println;
using std::string;
using std::vector;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;
using std::operator""s;

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

    using ecdt = e_cell_data_type;
    // const string sample_row_0 =
    //     R"(Iceland.png,png,8.35,600,800,72,,,,,,Team Iceland,"""Johnson,
    //     Volcano, Dusk""")";
    const vector<string> sample_row_texts_0{"Iceland.png"s,
                                            "png"s,
                                            "8.35"s,
                                            "600"s,
                                            "800"s,
                                            "72"s,
                                            ""s,
                                            ""s,
                                            ""s,
                                            ""s,
                                            ""s,
                                            "Team Iceland"s,
                                            R"("""Johnson, Volcano, Dusk""")"};
    const vector<ecdt> sample_row_types_0{
        ecdt::text,         ecdt::text,         ecdt::floating,
        ecdt::integer,      ecdt::integer,      ecdt::integer,
        ecdt::undetermined, ecdt::undetermined, ecdt::undetermined,
        ecdt::undetermined, ecdt::undetermined, ecdt::text,
        ecdt::tags};

    const vector<string> sample_csv_rows{
        R"(Filename,Type,Image Size (MB),Image X,Image Y,DPI,(Center) Coordinate,Favorite,Continent,Bit color,Alpha,Hockey Team,User Tags)",
        R"(Iceland.png,png,8.35,600,800,72,,,,,,Team Iceland,"""Johnson, Volcano, Dusk""")",
        R"(Italy.png,png,10.5,600,800,96,,Yes,Europe,,,,)",
        R"(Japan.jpeg,jpeg,26.4,600,800,600,"36° 00' N, 138° 00' E",,Asia,,,,"""Mt Fuji, Fog""")",
        R"(Calgary.tif,tiff,30.6,600,800,1200,"51.05011, -114.08529",Yes,,32,Y,Flames,"""Urban, Dusk""")",
        R"(Edmonton.jpg,jpeg,5.6,900,400,72,"53.55014, -113.46871",,,,,Oilers,)"};
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
        const string input = MyFixture::sample_header;
        const vector<string> expected = MyFixture::sample_header_fields;
        const parser::header_fields result = parser::parse_header(input);

        CHECK_TRUE(result.size() == expected.size());
        if (result.size() != expected.size()) {
            println(stderr, "result.size() {} != expected.size() {}",
                    result.size(), expected.size());
            println(stderr, "result: {}", result);
        }

        CHECK_TRUE(ranges::all_of(
            ranges::zip_view(result, expected), [](auto r_e_pair) {
                return r_e_pair.first.name == r_e_pair.second;
            }));

        CHECK_TRUE(ranges::all_of(result, [](auto h) {
            return h.data_type == e_cell_data_type::undetermined;
        }));
    }

    // SECTION("parse sample header as columns") {
    //     using std::operator""sv;

    //     auto parsed_columns = parse_header(MyFixture::sample_header);

    //     CHECK_TRUE(parsed_columns.mostly_equal(MyFixture::sample_columns));
    // }

    SECTION("find triple-quoted fields") {}
}

TEST_CASE(MyFixture, ParseRow) {
    SECTION("parse_data_row") {
        using str_cell_pair = std::pair<string, e_cell_data_type>;
        const string input = MyFixture::sample_row_0;
        const auto expected =
            ranges::zip_view(MyFixture::sample_row_texts_0,
                             MyFixture::sample_row_types_0) |
            views::transform([](std::pair<string, e_cell_data_type> pr) {
                return parser::data_field{pr.first, pr.second};
            }) |
            ranges::to<parser::data_fields>();
        const parser::data_fields result = parser::parse_data_row(input);
        auto r_it = result.begin();
        auto e_it = expected.begin();
        size_t index = 0;
        while (r_it != result.end() && e_it != expected.end()) {
            CHECK_TRUE(*r_it == *e_it);
            if (*r_it != *e_it) {
                println(stderr, "\nparse_data_row");
                println(stderr, "result[{}]:   {}", index, str(*r_it));
                println(stderr, "expected[{}]: {}\n", index, str(*e_it));
            }
            ++r_it;
            ++e_it;
            ++index;
        }
    }
    SECTION("row value types") {
        auto parsed_strings = fix_quoted_fields(MyFixture::sample_row_3);
        auto val_types = parser::row_value_types(parsed_strings);
        int i = 0;
        for (auto v : val_types) {
            println(stderr, "row value types - val_types[{}]: {}", i++, str(v));
        }
    }
}

TEST_CASE(MyFixture, ParseFile) {
    SECTION("parse_lines") {
        const vector<string> input = MyFixture::sample_csv_rows;
        parser::header_and_data result = parser::parse_lines(input);
        CHECK_TRUE(!result.header_fields_.empty());
        CHECK_TRUE(result.data_fields_vec_.size() == input.size() - 1);
    }
}
