#include "../cell_types.hpp"

#include <algorithm>
#include <concepts>
#include <iterator>
#include <ranges>
#include <string>
#include <type_traits>
#include <vector>

#include "../jt_concepts.hpp"
#include "../utility.hpp"
#include "CppUnitTestFramework.hpp"
#include "general_fixture.hpp"

namespace {
using std::string;
using std::vector;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;

struct MyFixture : general_fixture {
    const vector<e_cell_data_type> ecdt_vec_1 = {
        e_cell_data_type::undetermined,   e_cell_data_type::invalid,
        e_cell_data_type::floating,       e_cell_data_type::boolean,
        e_cell_data_type::integer,        e_cell_data_type::text,
        e_cell_data_type::geo_coordinate, e_cell_data_type::tags};
    const vector<e_cell_data_type> ecdt_vec_2{ecdt_vec_1};

    using ecdt_pair_t =
        cross_product_pair_t<e_cell_data_type, e_cell_data_type>;

    static consteval cross_product_output_t<e_cell_data_type> all_cdt_combos() {
        auto v1 = all_e_cell_data_types();
        auto v2 = all_e_cell_data_types();
        return cross_product<e_cell_data_type>(v1, v2);
    }

    const vector<ecdt_pair_t> all_e_cell_data_type_combos = all_cdt_combos();
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
