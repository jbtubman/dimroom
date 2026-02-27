#include <cstdlib>
#include <optional>
#include <print>
#include <string>
#include <vector>

#include "command_line.hpp"
#include "table.hpp"

using std::string;
using std::vector;

int main(int argc, char** argv) {
    using namespace jt;
    using std::println;

    command_line cl;

    vector<string> argv_sv(argv, argv + argc);
    auto filename_exp = cl.get_csv_filename(argc, argv_sv);
    if (!filename_exp) {
        println(
            stderr,
            "{}: please specify a CSV filename (like ../test/data/sample.csv)",
            argv_sv[0]);
        return EXIT_FAILURE;
    }

    auto filename = *filename_exp;
    command_handler ch;
    auto table_exp = ch.read_csv_file(filename);
    if (!table_exp) {
        println(stderr, "could not read CSV input file \"{}\"", filename);
        return EXIT_FAILURE;
    }

    table tbl = *table_exp;
    return cl.read_eval_print(tbl);
}
