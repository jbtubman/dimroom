#pragma once

#include <string>
#include <vector>

#include "coordinates.hpp"
#include "query.hpp"
#include "table.hpp"

// Class to implement handling commands from the user interface.
namespace jt {
using std::string;
using std::vector;

// Assumes both tags and values are strings.
struct tag_value {
    string tag;
    string value;
};

class command_handler {
   public:
    table read_csv_file(const string& filename) {
      return table();
    }

    bool write_csv_file(const string& filname, table) {
      return false;
    }

    query create_query(const string& query_text, table table_to_search) {
      return query();
    }

    query create_tags_query(vector<tag_value> tag_values, table table_to_search){
      return query();
    }

    query create_geo_query(coordinate location, polygon bounding_polygon,
                           table table_to_search){
      return query();
    }

    vector<row> run_query(query q, table table_to_search){
      return vector<row>();
    }

    void exit_app();
};
}  // namespace jt
