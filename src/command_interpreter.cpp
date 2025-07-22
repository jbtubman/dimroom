#include "command_interpreter.hpp"

#include <algorithm>
#include <expected>
#include <iostream>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "cell_types.hpp"
#include "contains.hpp"
#include "coordinates.hpp"
#include "query.hpp"
#include "table.hpp"
#include "utility.hpp"

namespace jt {
using std::cerr;
using std::endl;
using std::expected;
using std::regex;
using std::regex_match;
using std::regex_search;
using std::smatch;
using std::string;
using std::unexpected;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::ranges::views;
using ecdt = e_cell_data_type;
using std::pair;

enum class query_error { bad_format };

// Query pattern that matches comparison operators.
// If no operator is found, assume =.
// TODO: incorporate into queries.
// Later, for ANDing, separate clauses with &&.
const string new_query_pattern_s{
    R"-(^\s*query\s*\(\s*"([^"]*)"\s+((tags|inside|=|!=|<|<=|>|>=)\s+)?(.*)\))-"};
const regex new_query_pattern_rx{new_query_pattern_s};

string row_to_string(const row& rw) {
    std::ostringstream sout{};
    bool first_column = true;
    ranges::for_each(rw, [&first_column, &sout](const data_cell& dc) {
        if (!first_column) {
            sout << ",";
        }
        first_column = false;
        if (dc.value) {
            sout << cell_value_types_value_as_string(*(dc.value));
        } else {
            sout << "";
        }
    });

    return sout.str();
}

const string points_in_query_s{
    R"-(^\s*query\s*\(\s*"(.*)"\s+(?:(("(.*)")|(\((.*)\)))\s*)?\s*inside\b\s*(.+))-"};
const regex points_in_query_rx{points_in_query_s};

// m[1] = column name.
constexpr size_t points_in_query_column_name_idx{1};

// m[2] = all the coordinates after the "in".
constexpr size_t points_in_query_polygon_idx{7};

query::comparison str_to_comparison(const string& ss) {
    using qc = query::comparison;
    const string s = to_lower(ss);

    if (s == "=") return qc::equal_to;
    if (s == "!=") return qc::not_equal_to;
    if (s == ">") return qc::greater;
    if (s == "<") return qc::less;
    if (s == ">=") return qc::greater_equal;
    if (s == "<=") return qc::less_equal;
    if (s == "inside") return qc::inside;
    if (s == "tags") return qc::tags;
    if (s == "") return qc::equal_to;
    return qc::invalid;
}

expected_polygon_t parse_points_in_query(const string& query_line) {
    smatch query_match;
    const bool is_query_match =
        regex_match(query_line, query_match, points_in_query_rx);
    if (!is_query_match)
        return std::unexpected(convert_error::geo_coordinate_convert_error);

    // Split apart the polygon coordinates.
    string polygon_coords_s = trim(query_match[points_in_query_polygon_idx]);
    // Get rid of the trailing parenthesis.
    regex polgon_terminator_rx{R"-(\s*\)\s*$)-"};
    polygon_coords_s =
        regex_replace(polygon_coords_s, polgon_terminator_rx, ""s);
    // Regularize the separators.
    regex polygon_sep_rx{R"-(\)\s*\()-"};
    polygon_coords_s = regex_replace(polygon_coords_s, polygon_sep_rx, ") ("s);

    regex coord_delimiter{R"-(["\(\)])-"};

    // Parse each coordinate.
    // something is wrong with the split.
    auto polygon_strings = polygon_coords_s | views::split(R"-() ()-"s) |
                           ranges::to<vector<string>>();
    polygon_t poly_coords{};
    size_t i{0};
    ranges::for_each(polygon_strings.begin(), polygon_strings.end(),
                     [&poly_coords, &coord_delimiter, &i](string& pg_s) {
                         pg_s = regex_replace(pg_s, coord_delimiter, ""s);
                         auto expected_coord = s_to_geo_coordinate(pg_s);
                         if (expected_coord) {
                             poly_coords.push_back(*expected_coord);
                         }
                     });

    // Make sure there are at least 3 coords in the vector.
    if (poly_coords.size() < 3)
        return std::unexpected(convert_error::geo_coordinate_convert_error);

    return poly_coords;
}

void command_line::do_query(table& t, const string& query_line) {
    // Most of this stuff should be refactored elsewhere.
    const string query_prefix_s{R"-(^\s*query\s*)-"};

    // Used regex101.com to work this out.
    // https://regex101.com/r/5AgL7v/1
    // This one matches queries like:
    // query ("Filename" "Iceland.png")
    //  query ("Type"  "png")
    // query("Image X" 600)
    regex simple_query_rx{
        // R"(^\s*query\s*(\(\s*(".+")\s+((".+")|(.+))\s*\))\s*$)",
        R"-(^\s*query\s*\(\s*"(.*)"\s+(("(.*)")|(.*))\s*\)\s*$)-",
        // R"(^\s*query\s*.*$)",
        regex::icase};

    table::rows results{};

    smatch m;

    const bool possible_points_in_polygon_query =
        regex_match(query_line, m, points_in_query_rx);
    // If it is a point-in-polygon query, handle it specially.
    if (possible_points_in_polygon_query) {
        // TBD
        const auto possible_polygon = parse_points_in_query(query_line);
        if (possible_polygon) {
            const auto& polygn = *possible_polygon;
            const string column_name = m[points_in_query_column_name_idx];
            query q(t, column_name);
            results = q.point_in_polygon_match(polygn);
        }
    } else {
        // m[1] is the column name.
        // m[3] is the operator (may be empty).
        // m[4] is the query value.
        regex_search(query_line, m, new_query_pattern_rx);
        if (m.empty()) {
            println(stderr, "could not parse query \"{}\"", query_line);
            return;
            // Will need to change this when handling more complex queries.
        }

        const string column_name = m[1].str();
        // m[4] for string values; m[5] for integers.
        const string query_value_s =
            m[4].str().empty() ? m[5].str() : m[4].str();

        auto op = m[3];
        const query::comparison comp = str_to_comparison(op.str());

        ecdt col_type = t.column_type(column_name);
        query q(t, column_name, comp);
        results = q.execute(query_value_s);
    }

    // Print out the column names.
    bool first_field = true;
    string column_names_output{};
    ranges::for_each(t.header_fields_, [&first_field, &column_names_output](
                                           const parser::header_field& hf) {
        if (!first_field) {
            column_names_output.append(",");
        }
        first_field = false;
        column_names_output.append(hf.name);
    });
    println("{}", column_names_output);

    // print the rows.
    bool first_row = true;
    ranges::for_each(results, [](const row& rw) {
        string row_str = row_to_string(rw);
        println("{}", row_str);
    });
    println("{} rows found", results.size());
}
}  // namespace jt
