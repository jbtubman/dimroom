#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "command_line.hpp"
#include "google_test_fixture.hpp"

namespace {
using std::string;
using namespace jt;
using namespace std::string_literals;
using std::operator""s;
using std::array;
using std::pair;
using std::unique_ptr;
using std::vector;

struct command_interpreter_fixture : google_test_fixture {};
}  // namespace

TEST_F(command_interpreter_fixture, GetFilenameGetCSVFilenameString) {
    command_line cli{};
    int argc = 2;
    const vector<string> argv({"test", "./data/sample.csv"});

    auto ofilename = cli.get_csv_filename(argc, argv);
    EXPECT_TRUE(ofilename);
    const auto filename = *ofilename;
    EXPECT_TRUE(filename == argv[1]);
}
