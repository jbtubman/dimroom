#pragma once

// Class to do command-line I/O.

#include <algorithm>
#include <cstdlib>
#include <format>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <vector>

#include "command_handler.hpp"
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
using std::print;
using std::println;
using std::regex;
using std::regex_match;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

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
        "\"query (\"column name\" = value)\" - do a query",
        "\"exit\" - end program", "\"quit\" - end program",

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

    const string prompt{"command> "};

    int read_eval_print(table& table_to_use) {
        println(stderr, "Welcome to DimRoom");
        println(stderr, "Enter the command \"help\" for help.");
        cout << prompt;
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
                do_query(table_to_use, input_line);
            }

            else {
                cout << "command line \"" << input_line << "\" not understood"
                     << endl;
            }

            cout << prompt;
        }

        println("Goodbye.");
        return EXIT_SUCCESS;
    }
};
}  // namespace jt
