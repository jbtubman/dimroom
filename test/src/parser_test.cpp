#include "parser.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "../include/general_fixture.hpp"
#include "CppUnitTestFramework.hpp"
#include "cell_types.hpp"
#include "parse_utils.hpp"
#include "parser_formatter.hpp"

namespace {
using std::string;
using std::vector;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;
using std::operator""s;

struct MyFixture : general_fixture {
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
};
}  // namespace

using std::string;
using std::vector;

using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;

// TODO: proper tests for parser_test.cpp
TEST_CASE(MyFixture, ParseHeader) {
    SECTION("parse sample header") {
        const string input = MyFixture::sample_header;
        const vector<string> expected = MyFixture::sample_header_fields;
        const auto result = parser::parse_header(input);
        CHECK_TRUE(result.has_value());
        using pair_type = std::pair<parser::header_field, string>;

        CHECK_TRUE(result->size() == expected.size());

        CHECK_TRUE(ranges::all_of(
            ranges::zip_view(*result, expected), [](const pair_type& r_e_pair) {
                return r_e_pair.first.name == r_e_pair.second;
            }));

        CHECK_TRUE(ranges::all_of(*result, [](auto h) {
            return h.data_type == e_cell_data_type::undetermined;
        }));
    }
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
        auto result_ = parser::parse_data_row(input);
        CHECK_TRUE(result_.has_value());
        auto result = *result_;
        auto r_it = result.begin();
        auto e_it = expected.begin();
        size_t index = 0;
        while (r_it != result.end() && e_it != expected.end()) {
            CHECK_TRUE(*r_it == *e_it);
            ++r_it;
            ++e_it;
            ++index;
        }
    }
}

TEST_CASE(MyFixture, ParseFile) {
    SECTION("parse_lines") {
        const vector<string> input = MyFixture::sample_csv_rows;
        auto result_ = parse_lines(input);
        CHECK_TRUE(result_.has_value());
        parser::header_and_data result = *result_;
        CHECK_TRUE(!result.get_header_fields().empty());
        CHECK_TRUE(result.get_data_fields().size() == input.size() - 1);
    }

    SECTION("parse_lines from ifstream") {
        const string filename = MyFixture::csv_input_file;
        CHECK_TRUE(std::filesystem::exists(filename));
        auto fp = std::filesystem::relative(filename);
        auto cp = std::filesystem::current_path();
        std::ifstream ifs(fp);
        CHECK_TRUE(ifs.good());
        std::filesystem::path dotp(".");
    }
}

TEST_CASE(MyFixture, GetDataTypeForAllColumns) {
    SECTION("get_data_types_for_all_columns") {
        using ecdt = e_cell_data_type;
        const vector<string> input = MyFixture::sample_csv_rows;
        auto hd_ = parse_lines(input);
        CHECK_TRUE(hd_.has_value());
        const parser::header_and_data hd = *hd_;
        const parser::all_data_fields adf = hd.get_data_fields();
        const auto result_ = parser::get_data_types_for_all_columns(hd);
        const auto result = *result_;
        const vector<e_cell_data_type> expected = {// Filename
                                                   ecdt::text,
                                                   // Type
                                                   ecdt::text,
                                                   // Image Size (MB)
                                                   ecdt::floating,
                                                   // Image X,
                                                   ecdt::integer,
                                                   // Image Y,
                                                   ecdt::integer,
                                                   // DPI,
                                                   ecdt::integer,
                                                   // (Center) Coordinate,
                                                   ecdt::geo_coordinate,
                                                   // Favorite,
                                                   ecdt::boolean,
                                                   // Continent,
                                                   ecdt::text,
                                                   // Bit color,
                                                   ecdt::integer,
                                                   // Alpha,
                                                   ecdt::text,
                                                   // Hockey Team,
                                                   ecdt::text,
                                                   // User Tags
                                                   ecdt::tags};
        CHECK_TRUE(result.size() == expected.size());
        for (size_t i = 0; i < result.size(); i++) {
            CHECK_TRUE(result[i] == expected[i]);
        }
    }
}
