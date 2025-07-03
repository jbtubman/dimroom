#include "../column.hpp"

#include <string>

#include "../cell_types.hpp"
#include "CppUnitTestFramework.hpp"

namespace {
using std::string;
using namespace jt;

struct MyFixture {
    // Fill in.
};
}  // namespace

TEST_CASE(MyFixture, Column) {
    SECTION("creation") {
        using cv_type = cell_value_type_t<value_type::integer>;
        string nm{"Filename"};
        cv_type test_int{0};
        std::size_t pos{0};
        cell_type ct{test_int};
        column col{nm, pos, ct};
        CHECK_TRUE(col.name == nm);
        CHECK_TRUE(col.position == pos);
        // see if the optional value is there.
        CHECK_TRUE(col.column_type);
        auto col_t_t = *(col.column_type);
        // see if the optional value is the right thing.
        auto col_v = std::get<cv_type>(col_t_t);
        CHECK_TRUE(col_v == test_int);
    }
}

TEST_CASE(MyFixture, Columns) {
  SECTION("add some") {
    column col_0{"Filename", 0};
    column col_1{"Type", 1};
    columns colms;
    colms.add(col_0).add(col_1);

    auto filename_col = colms.find("Filename");
    CHECK_TRUE(filename_col);
    CHECK_TRUE(*filename_col == col_0);
    CHECK_TRUE(filename_col->name == col_0.name);
  }
}
