#include <format>
#include <print>
#include <regex>
#include <vector>
#include <cstdlib>
#include <optional>

#include "cell_types.hpp"
#include "command_interpreter.hpp"
#include "contains.hpp"
#include "coordinate_formatter.hpp"
#include "coordinates.hpp"
#include "table.hpp"

int main(int argc, char** argv) {
    using namespace jt;
    command_line cl;
    auto filename = cl.get_csv_filename(argc, argv);
    if (filename) {
        command_handler ch;
        table t = ch.read_csv_file(*filename);
        return cl.read_eval_print(t);
    } else {
        return EXIT_FAILURE;
    }
}
