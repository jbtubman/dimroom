#pragma once

#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <string>
#include <system_error>
#include <vector>

#include "coordinates.hpp"
#include "parser.hpp"
// #include "query.hpp"
#include "table.hpp"

// Class to implement handling commands from the user interface.
namespace jt {
using std::expected;
using std::println;
using std::string;
using std::vector;

namespace filesystem = std::filesystem;

// Assumes both tags and values are strings.
struct tag_value {
    string tag;
    string value;
};

struct query;

class command_handler {
   public:
    expected<table, table::error> read_csv_file(const string& filename) {
        {
            filesystem::path fpath_{filename};
            auto fpath = filesystem::absolute(fpath_);

            if (!filesystem::exists(fpath)) {
                println(stderr, "file \"{}\" does not exist", filename);
                return std::unexpected(table::error::file_exist_error);
            }
            std::ifstream instream(fpath);
            if (!instream) {
                println(stderr, "file \"{}\" could not be opened for input",
                        filename);
                return std::unexpected(table::error::file_read_error);
            }
        }

        // read in all the rows from the file.
        auto result_ex = table::make_table_from_file(filename);
        if (!result_ex) {
            return unexpected(result_ex.error());
        }

        return *result_ex;
    }

    bool write_csv_file(const string& filname, table) { return false; }

    void exit_app() {};
};
}  // namespace jt
