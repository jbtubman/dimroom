#include <string>
#include <type_traits>
#include <memory>
#include <algorithm>
#include <array>

#include "CppUnitTestFramework.hpp"
#include "../command_interpreter.hpp"
#include "general_fixture.hpp"

namespace {
using std::println;
using std::string;
using namespace jt;
using namespace std::string_literals;
using std::operator""s;
using std::unique_ptr;
using std::array;

struct MyFixture : general_fixture {
    // ...
};
}  // namespace jt

using namespace jt;
using std::string;

TEST_CASE(MyFixture, GetFilename) {
  SECTION("get_csv_filename string") {
    command_line cli{};
    int argc = 2;
    const vector<string>argv({"test", "./data/sample.csv"});

    auto ofilename = cli.get_csv_filename(argc, argv);
    CHECK_TRUE(ofilename);
    const auto filename = *ofilename;
    CHECK_TRUE(filename == argv[1]);
  }
}
