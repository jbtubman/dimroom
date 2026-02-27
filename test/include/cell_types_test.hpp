#pragma once

#include <algorithm>
#include <format>
#include <print>
#include <string>
#include <vector>

#include "cell_types.hpp"
#include "cell_types_formatter.hpp"
#include "google_test_fixture.hpp"
#include "utility.hpp"

namespace {
using std::format;
using std::println;
using std::string;
using std::vector;
using namespace jt;

/// @brief Returns a vector of all cell data types.
/// @return vector<e_cell_data_type>
inline constexpr vector<e_cell_data_type> all_e_cell_data_types() {
    return {e_cell_data_type::undetermined,   e_cell_data_type::invalid,
            e_cell_data_type::floating,       e_cell_data_type::boolean,
            e_cell_data_type::integer,        e_cell_data_type::text,
            e_cell_data_type::geo_coordinate, e_cell_data_type::tags};
}

}  // namespace

struct cell_types_fixture : google_test_fixture {
    const vector<e_cell_data_type> ecdt_vec_1 = {
        e_cell_data_type::undetermined,   e_cell_data_type::invalid,
        e_cell_data_type::floating,       e_cell_data_type::boolean,
        e_cell_data_type::integer,        e_cell_data_type::text,
        e_cell_data_type::geo_coordinate, e_cell_data_type::tags};
    const vector<e_cell_data_type> ecdt_vec_2{ecdt_vec_1};

    using ecdt_pair_t =
        cross_product_pair_t<e_cell_data_type, e_cell_data_type>;

    static /* consteval */ cross_product_output_t<e_cell_data_type>
    all_cdt_combos() {
        auto v1 = all_e_cell_data_types();
        auto v2 = all_e_cell_data_types();
        return cross_product<e_cell_data_type>(v1, v2);
    }

    vector<ecdt_pair_t> all_e_cell_data_type_combos = all_cdt_combos();
};

namespace {
using std::string;
using std::vector;

using std::println;
using namespace jt;
namespace ranges = std::ranges;
}  // namespace

TEST_F(cell_types_fixture, CellTypesOperatorOr) {
    ranges::for_each(cell_types_fixture::all_e_cell_data_type_combos,
                     [this](ecdt_pair_t pair) {
                         e_cell_data_type ecdt_1 = pair.first;
                         e_cell_data_type ecdt_2 = pair.second;
                         if (ecdt_1 == ecdt_2) {
                             EXPECT_TRUE(ecdt_1 == (ecdt_1 || ecdt_2));
                         } else if (ecdt_1 == e_cell_data_type::undetermined) {
                             EXPECT_TRUE(ecdt_2 == (ecdt_1 || ecdt_2));
                         } else if (ecdt_2 == e_cell_data_type::undetermined) {
                             EXPECT_TRUE(ecdt_1 == (ecdt_1 || ecdt_2));
                         } else {
                             EXPECT_TRUE(false);
                         }
                     });
}

// Cell types formatter tests.

TEST_F(cell_types_fixture, Undetermined) {
    e_cell_data_type input{e_cell_data_type::undetermined};
    string expected("\"undetermined\"");
    auto result = format("{}", input);
    EXPECT_TRUE(result == expected);
    if (result != expected) {
        println("expected: \"{}\"", expected);
        println("result:   \"{}\"", result);
    }
}

TEST_F(cell_types_fixture, UndeterminedLong) {
    e_cell_data_type input{e_cell_data_type::undetermined};
    string expected("\"jt::e_cell_data_type::undetermined\"");
    // const auto lf = long_fmt.c_str();
    auto result = format("{0:@}", input);
    EXPECT_TRUE(result == expected);
    if (result != expected) {
        println("expected: \"{}\"", expected);
        println("result:   \"{}\"", result);
    }
}

TEST_F(cell_types_fixture, Invalid) {
    e_cell_data_type input{e_cell_data_type::invalid};
    string expected("\"invalid\"");
    auto result = format("{}", input);
    EXPECT_TRUE(result == expected);
    if (result != expected) {
        println("expected: \"{}\"", expected);
        println("result:   \"{}\"", result);
    }
}

TEST_F(cell_types_fixture, InvalidLong) {
    e_cell_data_type input{e_cell_data_type::invalid};
    string expected("\"jt::e_cell_data_type::invalid\"");
    auto result = format("{0:@}", input);
    EXPECT_TRUE(result == expected);
    if (result != expected) {
        println("expected: \"{}\"", expected);
        println("result:   \"{}\"", result);
    }
}
