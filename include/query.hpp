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

/// @brief Query interface.
class query {
   public:
    table& t;
    string column_name{};

    query(table& tb, const string& c_name) : t{tb}, column_name{c_name} {}

    table::rows execute(const string& query_value_s);

    expected<string, int> do_string_query(table& tb,
                                          const string& query_string) {
        return std::unexpected(EXIT_FAILURE);
    }

    table::rows string_match(const string& query_value,
                             table::opt_rows rows_to_query = table::opt_rows{});

    table::rows integer_match(
        const string& query_value,
        table::opt_rows rows_to_query = table::opt_rows{});

    table::rows integer_match(
        int query_value, table::opt_rows rows_to_query = table::opt_rows{});

    table::rows boolean_match(
        bool query_value, table::opt_rows rows_to_query = table::opt_rows{});

    table::rows boolean_match(
        const string& query_value,
        table::opt_rows rows_to_query = table::opt_rows{});

    table::rows floating_match(
        float query_value, table::opt_rows rows_to_query = table::opt_rows{});

    table::rows floating_match(
        const string& query_value,
        table::opt_rows rows_to_query = table::opt_rows{});

    table::rows geo_query_match(
        const coordinate& coord,
        table::opt_rows rows_to_query = table::opt_rows{});

    table::rows geo_query_match(
        const string& coord,
        table::opt_rows rows_to_query = table::opt_rows{});

   private:
    auto vw_string_match(const string& query_value,
                         ranges::ref_view<table::rows> targets);

    auto vw_integer_match(int query_value,
                          ranges::ref_view<table::rows> targets);

    auto vw_boolean_match(bool query_value,
                          ranges::ref_view<table::rows> targets);

    auto vw_floating_match(float query_value,
                           ranges::ref_view<table::rows> targets);

    auto vw_geo_query_match(const coordinate& coord,
                            ranges::ref_view<table::rows> targets);
};
}  // namespace jt
