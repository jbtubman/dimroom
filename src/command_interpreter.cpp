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
using std::regex_replace;
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

// In this one, m[1] matches everything after the word "query" to the end of the
// line.
const string long_query_pattern_s{R"-(^\s*query\s*(\((.*)\))\s*$)-"};
const regex long_query_pattern_rx{long_query_pattern_s};

// Then we need to regularize any " && " strings in between the clauses,
// so they can be split apart in the following step.
const string and_separator_pattern_s{R"-((\)\s*&&\s*\())-"};
const regex and_separator_pattern_rx{and_separator_pattern_s};

// Query pattern that matches comparison operators.
// If no operator is found, assume =.
// TODO: incorporate into queries.
// Later, for ANDing, separate clauses with &&.
const string query_clause_pattern_s{
    R"-(\(\s*"([^"]*)"\s+((tags|inside|=|!=|<|<=|>|>=)\s+)?(.*)\))-"};
const regex query_clause_pattern_rx{query_clause_pattern_s};

const string alt_query_clause_pattern_s{R"-(\s*\(.+\)\s*$)-"};

const string new_query_pattern_s =
    ""s + R"-(^(?:\s*query\s*)?)-"
    + query_clause_pattern_s;
    // + alt_query_clause_pattern_s;
//{R"-(^\s*query\s*\(\s*"([^"]*)"\s+((tags|inside|=|!=|<|<=|>|>=)\s+)?(.*)\))-"};
const regex new_query_pattern_rx{new_query_pattern_s};

// This doesn't work.
const string compound_query_pattern_s =
    ""s + R"-(^\s*query\s*)-" + R"-(()-" + query_clause_pattern_s + R"-())-" +
    R"-(()-" + R"-(\s*&&\s*)-" + query_clause_pattern_s + R"-()*)-";
const regex compound_query_pattern_rx{compound_query_pattern_s};

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
    R"-(^(?:\s*query\s*)?\(\s*"(.*)"\s+(?:(("(.*)")|(\((.*)\)))\s*)?\s*inside\b\s*(.+))-"};
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

/// @brief Experimental attempt to parse ANDed queries.
/// @param t
/// @param query_line
void command_line::new_do_query(table& t, const string& query_line) {
    // First, match the long query pattern to get everything after the word
    // "query".
    smatch m;
    bool success = regex_match(query_line, m, long_query_pattern_rx);
    if (!success) {
        println(stderr, "new_do_query: could not match long_query_pattern_rx");
        return;
    }

    // Then split up the matched string on the ")\s*&&\s*(" part.
    // Subsitute a uniform ") && (" for any variations in spacing.
    const string uniform_clauses{
        regex_replace(m[1].str(), and_separator_pattern_rx, ") && ("s)};
    println(stderr, "\nnew_do_query - uniform_clauses:\n“{}”\n",
            uniform_clauses);

    vector<string> clauses_vec =
        uniform_clauses | views::split(" && "s) | ranges::to<vector<string>>();

    int i{0};
    for (const auto& s : clauses_vec) {
        println(stderr, "clauses_vec[{}]: “{}”", i++, s);
    }

    // Then do each query, feeding the previous results in to the next one.
    // Start out with the original input table with all the original rows.
    // Then reduce the number of rows searched, based on the query results.
    // The rows in the final table are the ANDed results.
    table query_table{t};
    println(stderr, "query_table.rows_ : {}\n", query_table.rows_);

    for (const auto& clause : clauses_vec) {
        query_table = do_one_query(query_table, clause);
        println(stderr, "query_table.rows_ : {}\n", query_table.rows_);
    }

    // Print out the column names.
    bool first_field = true;
    string column_names_output{};
    ranges::for_each(query_table.header_fields_, [&first_field, &column_names_output](
                                           const parser::header_field& hf) {
        if (!first_field) {
            column_names_output.append(",");
        }
        first_field = false;
        column_names_output.append(hf.name);
    });
    println("{}", column_names_output);

    const auto& results = query_table.rows_;

    // print the rows.
    bool first_row = true;
    ranges::for_each(results, [](const row& rw) {
        string row_str = row_to_string(rw);
        println("{}", row_str);
    });
    println("{} rows found", results.size());
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
        results = parse_non_poly_query(t, query_line);
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

/// @brief Take a query string and a set of current results and run the query on
/// those results.
/// @param query_table
/// @param query_clause
/// @return A copy of the table with only the result rows in it.
table command_line::_do_one_query(
    table&& query_table, const string& query_clause) {
    table::rows result_rows{};

    smatch m;

    // table query_table{t, rows_to_query};

    // table::opt_rows opt_row_to_query{rows_to_query};

    const bool possible_points_in_polygon_query =
        regex_match(query_clause, m, points_in_query_rx);
    // If it is a point-in-polygon query, handle it specially.
    if (possible_points_in_polygon_query) {
        // TBD
        const auto possible_polygon = parse_points_in_query(query_clause);
        if (possible_polygon) {
            const auto& polygn = *possible_polygon;
            const string column_name = m[points_in_query_column_name_idx];
            query q(query_table, column_name);
            result_rows = q.point_in_polygon_match(polygn);
        }
    } else {
        result_rows = parse_non_poly_query(query_table, query_clause);
    }
    query_table.rows_ = result_rows;

    return query_table;
}

/// @brief Take a query string and a set of current results and run the query on
/// those results.
/// @param t
/// @param query_clause
/// @param rows_to_query
/// @return A copy of the table with only the result rows in it.
table command_line::_do_one_query(
    const table& t, const string& query_clause) {
    table::rows result_rows{};

    smatch m;

    table query_table{t};

    return _do_one_query(table{t}, query_clause);

    // table::opt_rows opt_row_to_query{rows_to_query};

    // const bool possible_points_in_polygon_query =
    //     regex_match(query_clause, m, points_in_query_rx);
    // // If it is a point-in-polygon query, handle it specially.
    // if (possible_points_in_polygon_query) {
    //     // TBD
    //     const auto possible_polygon = parse_points_in_query(query_clause);
    //     if (possible_polygon) {
    //         const auto& polygn = *possible_polygon;
    //         const string column_name = m[points_in_query_column_name_idx];
    //         query q(query_table, column_name);
    //         result_rows = q.point_in_polygon_match(polygn);
    //     }
    // } else {
    //     result_rows = parse_non_poly_query(query_table, query_clause);
    // }
    // query_table.rows_ = result_rows;

    // return query_table;
}

jt::table::rows command_line::parse_non_poly_query(table& t,
                                                   const string& query_line) {
    jt::table::rows results;
    smatch m;

    // m[1] is the column name.
    // m[3] is the operator (may be empty).
    // m[4] is the query value.
    bool matched = regex_search(query_line, m, new_query_pattern_rx);
    if (!matched) {
    // if (m.empty()) {
        println(stderr, "could not parse query \"{}\"", query_line);
        return results;
        // Will need to change this when handling more complex queries.
    }

    const string column_name = m[1].str();
    // m[4] for string values; m[5] for integers.
    const string query_value_s = m[4].str().empty() ? m[5].str() : m[4].str();

    auto op = m[3];
    const query::comparison comp = str_to_comparison(op.str());

    ecdt col_type = t.column_type(column_name);
    query q(t, column_name, comp);
    results = q.execute(query_value_s);
    return results;
}
}  // namespace jt
