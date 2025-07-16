#include <cstdlib>
#include <format>
#include <optional>
#include <print>
#include <regex>
#include <string>
#include <vector>

#include "cell_types.hpp"
#include "command_interpreter.hpp"
#include "contains.hpp"
#include "coordinates.hpp"
#include "table.hpp"

using std::string;
using std::vector;

int main(int argc, char** argv) {
    using namespace jt;
    using std::println;

    command_line cl;

    vector<string> argv_sv(argv, argv + argc);
    auto filename = cl.get_csv_filename(argc, argv_sv);
    if (filename) {
        println(stderr, "filename: \"{}\"", *filename);
        command_handler ch;
        auto t = ch.read_csv_file(*filename);
        if (t) {
            return cl.read_eval_print(*t);
        } else {
            println(stderr, "could not read CSV input file \"{}\"", *filename);
            return EXIT_FAILURE;
        }
    } else {
        println(stderr,
                "{}: please specify a CSV filename (like ../test/data/sample.csv)",
                argv_sv[0]);
        return EXIT_FAILURE;
    }
}
