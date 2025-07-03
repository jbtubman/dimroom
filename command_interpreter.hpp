#pragma once

// Class to do command-line I/O.

#include <optional>
#include <string>
#include <cstdlib>

#include "command_handler.hpp"
#include "query.hpp"
#include "table.hpp"

namespace jt {
using std::string;

class command_line {
    // TODO.
   public:
    std::optional<string> get_csv_filename(int argc, char** argv) {
        // TODO
        return std::nullopt;
    }

    string read_command(/* should have a stream or handle here. */);
    string evaluate_command(const string& command_string);
    void print_result(const string& result);

    int read_eval_print(table table_to_use) {
      // TODO
      return EXIT_FAILURE;
    }
};
}  // namespace jt
