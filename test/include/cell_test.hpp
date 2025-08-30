#pragma once

#include <algorithm>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "cell.hpp"
#include "cell_types.hpp"
#include "google_test_fixture.hpp"
#include "parser.hpp"
#include "utility.hpp"

namespace {
using std::string;
using std::vector;
using namespace jt;

struct cell_test_fixture : google_test_fixture {
    // ...
    using data_field = parser::data_field;
    using ecdt = e_cell_data_type;

    const data_field sample_text_df{"Iceland.png", ecdt::text};
    const data_field sample_float_df{"8.35", ecdt::floating};
    const data_field sample_integer_df{"800", ecdt::integer};
    const data_field sample_dec_coord_df{R"("51.05011, -114.08529")",
                                         ecdt::geo_coordinate};
    const data_field sample_dms_coord_df{R"("36° 00' N, 138° 00' E")",
                                         ecdt::geo_coordinate};
    const data_field sample_false_boolean_df{"No", ecdt::boolean};
    const data_field sample_true_boolean_df{"Yes", ecdt::boolean};
    const data_field sample_tag_df{R"("""Johnson, Volcano, Dusk""")",
                                   ecdt::tags};
    const data_field sample_invalid_df{"INVALID", ecdt::invalid};
    const data_field sample_undeterminated_df{"UNDETERMINED",
                                              ecdt::undetermined};

    const vector<data_field> value_dfs{
        sample_text_df,         sample_float_df,     sample_integer_df,
        sample_dec_coord_df,    sample_dms_coord_df, sample_false_boolean_df,
        sample_true_boolean_df, sample_tag_df};

    const vector<data_field> empty_dfs{sample_invalid_df,
                                       sample_undeterminated_df};
};
}  // namespace

namespace {
using std::regex;
using std::string;
using std::string_view;
using std::vector;

namespace ranges = std::ranges;
namespace views = std::views;

using namespace jt;

// TODO: Surely there is something in the STL to do this.
template <class Collection>
inline auto find_final(Collection&& collection) {
    auto col = std::forward<Collection>(collection);
    auto current = col.begin();
    auto next = current;
    next++;
    while (next != col.end()) {
        current++;
        next++;
    }
    return current;
}
}  // namespace

TEST_F(cell_test_fixture, MakeCellValueTypeText) {
    const auto& input = cell_test_fixture::sample_text_df;
    const auto result = data_cell::make_cell_value_type(input);
    EXPECT_TRUE(result);
    EXPECT_TRUE(*result == "Iceland.png");
}

TEST_F(cell_test_fixture, MakeCellValueTypeFloating) {
    const auto& input = cell_test_fixture::sample_float_df;
    const cell_value_type result = data_cell::make_cell_value_type(input);
    EXPECT_TRUE(result);
    using foo = decltype(*result);
    const float result_f = std::get<float>(*result);
    EXPECT_TRUE(is_close(result_f, 8.35));
}

TEST_F(cell_test_fixture, MakeCellValueTypeInteger) {
    const auto& input = cell_test_fixture::sample_integer_df;
    const auto result = data_cell::make_cell_value_type(input);
    EXPECT_TRUE(result);
    const int result_i = std::get<int>(*result);
    EXPECT_TRUE(result_i == 800);
}

TEST_F(cell_test_fixture, MakeCellValueTypeDecimalGeoCoordinate) {
    const auto& input = cell_test_fixture::sample_dec_coord_df;
    const auto result = data_cell::make_cell_value_type(input);
    EXPECT_TRUE(result);
}

// This doesn't actually seem to do any checks...
// TODO: Fix cell splitting check.
TEST_F(cell_test_fixture, CellSplitting) {
    using std::operator""sv;

    string_view sample_row_sv{cell_test_fixture::sample_row_3};

    auto fields_view = sample_row_sv | views::split(","sv) |
                       views::transform([](auto field_sv) {
                           return string(field_sv.begin(), field_sv.end());
                       });

    auto raw_field_strings = fields_view | ranges::to<vector>();

    // Glue together the tags fields.
    regex start_tag_rx{R"(^""".*)"};
    auto start_tag_fn = [start_tag_rx](const string& s) {
        return regex_match(s, start_tag_rx);
    };

    regex end_tag_rx{R"(.*"""$)"};
    auto end_tag_fn = [end_tag_rx](const string& s) {
        return regex_match(s, end_tag_rx);
    };
    auto beginning_tags = std::find_if(raw_field_strings.begin(),
                                       raw_field_strings.end(), start_tag_fn);

    if (beginning_tags != raw_field_strings.end()) {
        // Look for the end.
        auto start_tags = beginning_tags;
        ++start_tags;  // make sure it isn't the end.
        auto ending_tags =
            std::find_if(start_tags, raw_field_strings.end(), end_tag_fn);

        if (ending_tags != raw_field_strings.end()) {
            // glue them together.
            auto the_end = ending_tags;
            ++the_end;
            vector<string> tag_vec(beginning_tags, the_end);

            string initial{""};
            auto res =
                ranges::fold_left(tag_vec, initial, [](string acc, string s) {
                    if (acc.empty()) {
                        return s;
                    } else {
                        return acc + "," + s;
                    }
                });
        }
    }
}

TEST_F(cell_test_fixture, CellConverting1) {
    auto input_ = parse_lines(cell_test_fixture::sample_csv_rows);
    EXPECT_TRUE(input_.has_value());
    const parser::header_and_data input = *input_;
    const auto all_data_cells =
        data_cell::make_all_data_cells(input.all_data_fields);
    const auto first_row_data_cells =
        data_cell::make_data_cells(input.all_data_fields[0]);
    const auto first_result = first_row_data_cells[0];
    EXPECT_TRUE(first_result.value);
    const auto val = *first_result.value;
    const auto s = std::get<string>(val);
    EXPECT_TRUE(s == "Iceland.png");
}

TEST_F(cell_test_fixture, CellConvertingJapan) {
    auto input_ = parse_lines(cell_test_fixture::sample_csv_rows);
    EXPECT_TRUE(input_.has_value());
    const parser::header_and_data input = *input_;
    const auto all_data_cells =
        data_cell::make_all_data_cells(input.all_data_fields);
    // Japan is the third row.
    const auto cells = all_data_cells[2];
    // R"(Filename,Type,Image Size (MB),Image X,Image Y,DPI,(Center)
    // Coordinate,Favorite,Continent,Bit color,Alpha,Hockey Team,User Tags)"
    // R"(Japan.jpeg,jpeg,26.4,600,800,600,"36° 00' N, 138° 00'
    // E",,Asia,,,,"""Mt Fuji, Fog""")",
    const auto filename = cells[0];
    EXPECT_TRUE(filename);
    EXPECT_TRUE(true);
    EXPECT_TRUE(filename.get_string() == "Japan.jpeg");
}
