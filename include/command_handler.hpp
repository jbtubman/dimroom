#pragma once

#include <expected>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <vector>

#include "coordinates.hpp"
#include "parser.hpp"
#include "table.hpp"

// Class to implement handling commands from the user interface.
namespace jt {
using std::expected;
using std::println;
using std::string;
using std::vector;

namespace filesystem = std::filesystem;

class command_handler {
   public:
#if defined(_WIN64)
    using IFSTREAM = std::wifstream;
#else
    using IFSTREAM = std::ifstream;
#endif

    /// @brief Attempts to read in the CSV file indicated by the filename.
    /// @param filename
    /// @return A jt::table if there is such a file and it can be read and
    /// parsed; otherwise an error.
    expected<table, parser::error> read_csv_file(const string& filename) {
        // Error checking is done in this scope.
        {
            filesystem::path fpath_{filename};
            auto fpath = filesystem::absolute(fpath_);

            if (!filesystem::exists(fpath)) {
                println(stderr, "file \"{}\" does not exist", filename);
                return std::unexpected(parser::error::file_exist_error);
            }
            IFSTREAM instream(fpath);
            if (!instream) {
                println(stderr, "file \"{}\" could not be opened for input",
                        filename);
                return std::unexpected(parser::error::file_read_error);
            }
        }

        // read in all the rows from the file.
        auto result_ex = table::make_table_from_file(filename);
        if (!result_ex) {
            return unexpected(result_ex.error());
        }

        return *result_ex;
    }
};
}  // namespace jt
