#pragma once

// Class to do command-line I/O.

#include <algorithm>
#include <cstdlib>
#include <expected>
#include <format>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "command_handler.hpp"
#include "coordinates.hpp"
#include "dimroomConfig.h"
#include "query.hpp"
#include "table.hpp"
#include "utility.hpp"

namespace jt {
using std::optional;
using std::string;
using std::vector;
using namespace std::string_literals;
using std::operator""s;
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::flush;
using std::getline;
using std::pair;
using std::print;
using std::println;
using std::regex;
using std::regex_match;
namespace ranges = std::ranges;
namespace views = std::ranges::views;
using expected_polygon_t = std::expected<polygon_t, convert_error>;

class command_line {
    // TODO.
   public:
    // This is intended to extract the name of the input CSV file from the
    // command line.
    optional<string> get_csv_filename(int argc, const vector<string>& argv) {
        if (argc <= 1) {
            return std::nullopt;
        }

        const string filename{argv[1]};
        return filename;
    }

    const vector<string> help_strings{
        "\"describe\" - describe the table",
        "\"query (\"column name\" operator value)\" - do a regular query",
        "\tavailable operators are = != > >= < <=",
        "\"query (\"column name\" inside (coordinate) (coordinate) "
        "(coordinate)...)\" "
        "- look for coordinates inside a polygon of at least 3 points ",
        "\"query (\"column name\" tags \"tag1\" \"tag2\"...)\" - look for tags "
        "in a column",
        "\"exit\" - end program",
        "\"quit\" - end program",
        "\"help\" - print help message"};

    const regex quit_cmd_rx{R"(^\s*(quit|exit)\b.*)", regex::icase};
    const regex help_cmd_rx{R"(^\s*help\b.*)", regex::icase};
    const regex describe_cmd_rx{R"(^\s*describe\b.*)", regex::icase};
    const regex query_cmd_rx{R"(^\s*query\b\s+\(.*)", regex::icase};

    string read_command(/* should have a stream or handle here. */) {
        return "TBD"s;
    };
    string evaluate_command(const string& command_string) { return "TBD"s; };

    void print_result(const string& result) const { cout << result << endl; };

    void print_help() const {
        ranges::for_each(help_strings,
                         [](const string& s) { cerr << s << endl; });
    }

    void describe_table(table& t) {
        ranges::for_each(t.header_fields_, [](const parser::header_field& hf) {
            println("Column Name: \"{}\"; Column Type : {}", hf.name,
                    hf.data_type);
        });
    }

    void do_query(table& t, const string& query_line);

    /// @brief Experimental attempt to parse ANDed queries.
    /// @param t
    /// @param query_line
    void new_do_query(table& t, const string& query_line);

    table _do_one_query(const table& t, const string& query_clause);

    table _do_one_query(table&& t, const string& query_clause);

    /// @brief Take a query string and a set of current results and run the
    /// query on those results.
    /// @param t
    /// @param query_clause
    /// @param rows_to_query
    /// @return
    template <class TABLE>
    table do_one_query(TABLE&& t, const string& query_clause) {
        return _do_one_query(std::forward<TABLE>(t), query_clause);
    }

    int read_eval_print(table& table_to_use) {
        println(stderr, "Welcome to DimRoom");
        println(stderr, "Enter the command \"help\" for help.");
        const string prompt_str = std::format(
            "dimroom-{}.{}> ", dimroom_VERSION_MAJOR, dimroom_VERSION_MINOR);
        cout << prompt_str;
        string input_line;
        while (getline(cin, input_line)) {
            trim(input_line);
            if (regex_match(input_line, quit_cmd_rx)) {
                println("Goodbye.");
                return EXIT_SUCCESS;
            }

            if (std::regex_match(input_line, help_cmd_rx)) {
                print_help();
            } else if (regex_match(input_line, describe_cmd_rx)) {
                describe_table(table_to_use);
            } else if (regex_match(input_line, query_cmd_rx)) {
                // do_query(table_to_use, input_line);
                new_do_query(table_to_use, input_line);
            }

            else {
                cout << "command line \"" << input_line << "\" not understood"
                     << endl;
            }

            cout << prompt_str;
        }

        println("Goodbye.");
        return EXIT_SUCCESS;
    }

    jt::table::rows parse_non_poly_query(table& t, const string& query_line);

    jt::table::rows parse_queries(table& t, const vector<string>& query_lines);
};

expected_polygon_t parse_points_in_query(const string& query_line);

}  // namespace jt
