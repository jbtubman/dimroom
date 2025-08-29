#include "cell_types.hpp"
#include "cell_types_formatter.hpp"

#include <algorithm>
#include <concepts>
#include <format>
#include <iterator>
#include <print>
#include <ranges>
#include <string>
#include <type_traits>
#include <vector>

#include "../include/general_fixture.hpp"
#include "CppUnitTestFramework.hpp"
#include "jt_concepts.hpp"
#include "utility.hpp"

namespace {
using std::string;
using std::format;
using std::vector;
using std::println;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;

/// @brief Returns a vector of all cell data types.
/// @return vector<e_cell_data_type>
inline constexpr vector<e_cell_data_type> all_e_cell_data_types() {
    return {e_cell_data_type::undetermined,   e_cell_data_type::invalid,
            e_cell_data_type::floating,       e_cell_data_type::boolean,
            e_cell_data_type::integer,        e_cell_data_type::text,
            e_cell_data_type::geo_coordinate, e_cell_data_type::tags};
}
struct MyFixture : general_fixture {
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
}  // namespace

using std::string;
using std::vector;

using std::println;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;

TEST_CASE(MyFixture, CellTypes) {
    SECTION("operator||") {
        ranges::for_each(
            MyFixture::all_e_cell_data_type_combos, [this](ecdt_pair_t pair) {
                e_cell_data_type ecdt_1 = pair.first;
                e_cell_data_type ecdt_2 = pair.second;
                if (ecdt_1 == ecdt_2) {
                    CHECK_TRUE(ecdt_1 == (ecdt_1 || ecdt_2));
                } else if (ecdt_1 == e_cell_data_type::undetermined) {
                    CHECK_TRUE(ecdt_2 == (ecdt_1 || ecdt_2));
                } else if (ecdt_2 == e_cell_data_type::undetermined) {
                    CHECK_TRUE(ecdt_1 == (ecdt_1 || ecdt_2));
                } else {
                    CHECK_TRUE(false);
                }
            });
    }
}

TEST_CASE(MyFixture, CellTypesFormatter) {
    SECTION("undetermined") {
        e_cell_data_type input{e_cell_data_type::undetermined};
        string expected("\"undetermined\"");
        auto result = format("{}", input);
        CHECK_TRUE(result == expected);
        if (result != expected) {
            println("expected: \"{}\"", expected);
            println("result:   \"{}\"", result);
        }
    }
    SECTION("undetermined long") {
        e_cell_data_type input{e_cell_data_type::undetermined};
        string expected("\"jt::e_cell_data_type::undetermined\"");
        // const auto lf = long_fmt.c_str();
        auto result = format("{0:@}", input);
        CHECK_TRUE(result == expected);
        if (result != expected) {
            println("expected: \"{}\"", expected);
            println("result:   \"{}\"", result);
        }
    }

    SECTION("invalid") {
        e_cell_data_type input{e_cell_data_type::invalid};
        string expected("\"invalid\"");
        auto result = format("{}", input);
        CHECK_TRUE(result == expected);
        if (result != expected) {
            println("expected: \"{}\"", expected);
            println("result:   \"{}\"", result);
        }
    }
    SECTION("invalid long") {
        e_cell_data_type input{e_cell_data_type::invalid};
        string expected("\"jt::e_cell_data_type::invalid\"");
        auto result = format("{0:@}", input);
        CHECK_TRUE(result == expected);
        if (result != expected) {
            println("expected: \"{}\"", expected);
            println("result:   \"{}\"", result);
        }
    }
}
