#pragma once

#include <algorithm>
#include <cstdlib>
#include <expected>
#include <format>
#include <iostream>
#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <vector>

#include "command_handler.hpp"
#include "coordinates.hpp"
#include "table.hpp"
#include "utility.hpp"

// Class to query tables for matching information goes here.
namespace jt {
using std::expected;
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

auto vw_string_match(table& t, const string& col_name,
                     const string& query_value,
                     ranges::ref_view<table::rows> targets);

table::rows string_match(
    table& t, const string& col_name, const string& query_value,
    table::opt_rows rows_to_query = table::opt_rows{});

auto vw_integer_match(table& t, const string& col_name,
                      const string& query_value,
                      ranges::ref_view<table::rows> targets);

table::rows integer_match(
    table& t, const string& col_name, const string& query_value,
    table::opt_rows rows_to_query = table::opt_rows{});

table::rows integer_match(
    table& t, const string& col_name, int query_value,
    table::opt_rows rows_to_query = table::opt_rows{});

auto vw_boolean_match(table& t, const string& col_name, bool query_value,
                      ranges::ref_view<table::rows> targets);

table::rows boolean_match(
    table& t, const string& col_name, bool query_value,
    table::opt_rows rows_to_query = table::opt_rows{});

table::rows boolean_match(
    table& t, const string& col_name, const string& query_value,
    table::opt_rows rows_to_query = table::opt_rows{});

auto vw_floating_match(table& t, const string& col_name, float query_value,
                       ranges::ref_view<table::rows> targets);

table::rows floating_match(
    table& t, const string& col_name, float query_value,
    table::opt_rows rows_to_query = table::opt_rows{});

table::rows floating_match(
    table& t, const string& col_name, const string& query_value,
    table::opt_rows rows_to_query = table::opt_rows{});

auto vw_geo_query_match(table& t, const string& col_name,
                        const coordinate& coord,
                        ranges::ref_view<table::rows> targets);

table::rows geo_query_match(
    table& t, const string& col_name, const coordinate& coord,
    table::opt_rows rows_to_query = table::opt_rows{});

table::rows geo_query_match(
    table& t, const string& col_name, const string& coord,
    table::opt_rows rows_to_query = table::opt_rows{});



struct query {
    table& table_ref;
    string column_name{};
    vector<string> arguments{};

    query(table& t, const string& c_name, const vector<string> args)
        : table_ref{t}, column_name{c_name}, arguments{args} {}

    query(table& t, const string& c_name) : table_ref{t}, column_name{c_name} {}

    query(table& t) : table_ref{t} {}

    expected<string, int> do_string_query(table& t,
                                          const string& query_string) {
        return std::unexpected(EXIT_FAILURE);
    }
};
}  // namespace jt
