#include "../column.hpp"

#include <string>

#include "../cell_types.hpp"
#include "CppUnitTestFramework.hpp"
#include "general_fixture.hpp"

namespace {
using std::string;
using namespace jt;

struct MyFixture : general_fixture {
    const vector<column> sample_cols = {column("Filename", 0),
                                        column("Type", 1),
                                        column("Image Size (MB)", 2),
                                        column("Image X", 3),
                                        column("Image Y", 4),
                                        column("DPI", 5),
                                        column("(Center) Coordinate", 6),
                                        column("Favorite", 7),
                                        column("Continent", 8),
                                        column("Bit color", 9),
                                        column("Alpha", 10),
                                        column("Hockey Team", 11),
                                        column("User Tags", 12)};
    using cols_view_t = decltype(views::all(sample_cols));
    cols_view_t sample_cols_view = views::all(sample_cols);
    const columns sample_columns{sample_cols_view};
};
}  // namespace

TEST_CASE(MyFixture, Column) {
    SECTION("creation") {
        using cv_type = cpp_cell_data_type_t<e_cell_data_type::integer>;
        string nm{"Filename"};
        cv_type test_int{0};
        std::size_t pos{0};
        cell_value_type ct{test_int};
        column col{nm, pos, ct};
        CHECK_TRUE(col.name == nm);
        CHECK_TRUE(col.position == pos);
        // see if the optional value is there.
        CHECK_TRUE(col.value);
        auto col_t_t = *(col.value);
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
