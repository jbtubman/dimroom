#include "command_interpreter.hpp"

#include <expected>
#include <iostream>
#include <print>
#include <regex>
#include <sstream>
#include <string>

#include "cell_types.hpp"
#include "query.hpp"
#include "table.hpp"
#include "utility.hpp"

namespace jt {
using std::cerr;
using std::endl;
using std::expected;
using std::println;
using std::regex;
using std::regex_match;
using std::regex_search;
using std::smatch;
using std::string;
using std::unexpected;

enum class query_error { bad_format };

string row_to_string(const data_cells& row_dcs) {
    std::ostringstream sout{};
    bool first_column = true;
    ranges::for_each(row_dcs, [&first_column, &sout](const data_cell& dc) {
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

void command_line::do_query(table& t, const string& query_line) {
    println("do the query: \"{}\"", query_line);

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

    smatch m;
    // m[1] is the column name.
    // m[4] is the query value.

    regex_search(query_line, m, simple_query_rx);
    if (m.empty()) {
        println(stderr, "could not parse query \"{}\"", query_line);
        return;
        // Will need to change this when handling more complex queries.
    }

    int match_count = 0;
    for (auto it = m.begin(); it != m.end(); ++it) {
        const auto val = *it;
        cerr << "m[" << match_count++ << "] = \"" << val << "\"" << endl;
    }

    const string column_name = m[1].str();
    // m[4] for string values; m[5] for integers.
    const string query_value_s = m[4].str().empty() ? m[5].str() : m[4].str();
    table::cell_rows results;
    // Need to determine the column's value type and dispatch the query
    // appropriately.
    if (t.is_text(column_name)) {
        results = string_match(t, column_name, query_value_s);
    } else if (t.is_boolean(column_name)) {
        const auto query_value = s_to_boolean(query_value_s);
        if (query_value) {
            const bool query_b = *query_value;
            results = boolean_match(t, column_name, query_b);
        }
    } else if (t.is_floating(column_name)) {
    } else if (t.is_geo_coordinate(column_name)) {
    } else if (t.is_integer(column_name)) {
        const auto query_value = std::stoi(query_value_s);
        results = integer_match(t, column_name, query_value);
    } else if (t.is_tags(column_name)) {
    } else {
        // Invalid?
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

    using zzz = decltype(results[0]);
    bool first_row = true;
    ranges::for_each(results, [](const data_cells& row_dcs) {
        string row_str = row_to_string(row_dcs);
        println("{}", row_str);
    });
    println("{} rows found", results.size());
}
}  // namespace jt
