#include "command_interpreter.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "CppUnitTestFramework.hpp"
#include "contains.hpp"
#include "coordinates.hpp"
#include "general_fixture.hpp"

namespace {
using std::string;
using namespace jt;
using namespace std::string_literals;
using std::operator""s;
using std::array;
using std::pair;
using std::unique_ptr;
using std::vector;

struct MyFixture : general_fixture {
    // ...
    using test_pair = pair<coordinate, polygon_t>;

    const string valid_point_in_query_s{
        R"-(query ("(Center) Coordinate" (51.05011, -144.08529) in (51.129, -114.010) (50.742, -113.948) (50.748, -113.867) ))-"};

    const test_pair valid_point_in_query{
        make_coordinate("(51.05011, -144.08529)"s),
        *parse_points_in_query(valid_point_in_query_s)};

    const string another_query_s{
        R"-(query ("(Center) Coordinate" (50.00000, 0.00000) in (60.00000, 0.00000) (40.00000, 40.00000) (40.00000, -40.00000) ))-"};

    const test_pair another_query{make_coordinate("(50.00000, 0.00000)"),
                                  *parse_points_in_query(another_query_s)};

    const string should_be_inside_1_s{
        R"-(query ("(Center) Coordinate" (1.00000, 1.00000) in (5.00000, 5.00000) (5.00000, -5.00000) (-5.00000, -5.00000)  (-5.00000, 5.00000) ))-"};

    // this is the same for the rest of the queries.
    const expected_polygon_t e_polygon =
        parse_points_in_query(should_be_inside_1_s);

    const test_pair should_be_inside_1{make_coordinate("(1.00000, 1.00000)"s),
                                       *e_polygon};

    const string should_be_inside_2_s{
        R"-(query ("(Center) Coordinate" (-1.00000, 1.00000) in (5.00000, 5.00000) (5.00000, -5.00000) (-5.00000, -5.00000)  (-5.00000, 5.00000) ))-"};

    const test_pair should_be_inside_2{make_coordinate("(-1.00000, 1.00000)"s),
                                       *e_polygon};

    const string should_be_inside_3_s{
        R"-(query ("(Center) Coordinate" (1.00000, -1.00000) in (5.00000, 5.00000) (5.00000, -5.00000) (-5.00000, -5.00000)  (-5.00000, 5.00000) ))-"};

    const test_pair should_be_inside_3{make_coordinate("(1.00000, -1.00000)"s),
                                       *e_polygon};
    const string should_be_inside_4_s{
        R"-(query ("(Center) Coordinate" (-1.00000, -1.00000) in (5.00000, 5.00000) (5.00000, -5.00000) (-5.00000, -5.00000)  (-5.00000, 5.00000) ))-"};

    const test_pair should_be_inside_4{make_coordinate("(-1.00000, -1.00000)"s),
                                       *e_polygon};

    const vector<test_pair> should_be_inside{
        should_be_inside_1, should_be_inside_2, should_be_inside_3,
        should_be_inside_4};

    const string should_be_outside_1_s{
        R"-(query ("(Center) Coordinate" (10.00000, 0.00000) in (5.00000, 5.00000) (5.00000, -5.00000) (-5.00000, -5.00000)  (-5.00000, 5.00000) ))-"};

    const test_pair should_be_outside_1{make_coordinate("(10.00000, 0.00000)"s),
                                        *e_polygon};

    const string should_be_outside_2_s{
        R"-(query ("(Center) Coordinate" (10.00000, 10.00000) in (5.00000, 5.00000) (5.00000, -5.00000) (-5.00000, -5.00000)  (-5.00000, 5.00000) ))-"};

    const test_pair should_be_outside_2{
        make_coordinate("(10.00000, 10.00000)"s), *e_polygon};

    const string should_be_outside_3_s{
        R"-(query ("(Center) Coordinate" (0.00000, 10.00000) in (5.00000, 5.00000) (5.00000, -5.00000) (-5.00000, -5.00000)  (-5.00000, 5.00000) ))-"};

    const test_pair should_be_outside_3{make_coordinate("(0.00000, 10.00000)"s),
                                        *e_polygon};

    const string should_be_outside_4_s{
        R"-(query ("(Center) Coordinate" (-10.00000, 10.00000) in (5.00000, 5.00000) (5.00000, -5.00000) (-5.00000, -5.00000)  (-5.00000, 5.00000) ))-"};

    const test_pair should_be_outside_4{
        make_coordinate("(-10.00000, 10.00000)"s), *e_polygon};

    const string should_be_outside_5_s{
        R"-(query ("(Center) Coordinate" (-10.00000, 0.00000) in (5.00000, 5.00000) (5.00000, -5.00000) (-5.00000, -5.00000)  (-5.00000, 5.00000) ))-"};

    const test_pair should_be_outside_5{
        make_coordinate("(-10.00000, 0.00000)"s), *e_polygon};

    const string should_be_outside_6_s{
        R"-(query ("(Center) Coordinate" (-10.00000, -10.00000) in (5.00000, 5.00000) (5.00000, -5.00000) (-5.00000, -5.00000)  (-5.00000, 5.00000) ))-"};

    const test_pair should_be_outside_6{
        make_coordinate("(-10.00000, -10.00000)"s), *e_polygon};

    const vector<test_pair> should_be_outside{
        should_be_outside_1, should_be_outside_2, should_be_outside_3,
        should_be_outside_4, should_be_outside_5, should_be_outside_6};
};
}  // namespace

using namespace jt;
using std::string;

TEST_CASE(MyFixture, GetFilename) {
    SECTION("get_csv_filename string") {
        command_line cli{};
        int argc = 2;
        const vector<string> argv({"test", "./data/sample.csv"});

        auto ofilename = cli.get_csv_filename(argc, argv);
        CHECK_TRUE(ofilename);
        const auto filename = *ofilename;
        CHECK_TRUE(filename == argv[1]);
    }
}

TEST_CASE(MyFixture, PointInQueries) {
    SECTION("parse valid line") {
        expected_polygon_t result =
            parse_points_in_query(MyFixture::another_query_s);
        CHECK_TRUE(result);
        polygon_t polygon = *result;
        auto first_coord = make_coordinate("(50.00000, 0.00000)"s);
        auto in_result = point_in_polygon(first_coord, polygon);
        CHECK_TRUE(in_result);
    }

    SECTION("queries in polygon") {
        for (const test_pair& tp : MyFixture::should_be_inside) {
            polygon_t polygn = tp.second;
            auto first_coord = tp.first;
            auto in_result = point_in_polygon(first_coord, polygn);
            CHECK_TRUE(in_result);
        }
    }

    SECTION("queries outside polygon") {
        for (const test_pair& tp : MyFixture::should_be_outside) {
            auto first_coord = tp.first;
            auto polygn = tp.second;
            auto in_result = point_in_polygon(first_coord, polygn);
            CHECK_FALSE(in_result);
        }
    }
}
