#include "command_handler.hpp"

#include <string>
#include <vector>

#include "coordinates.hpp"
#include "query.hpp"
#include "table.hpp"

namespace jt {

query command_handler::create_query(const string& query_text,
                                    table table_to_search) {
    return query(table_to_search);
}

query command_handler::create_tags_query(vector<tag_value> tag_values,
                                         table table_to_search) {
    return query(table_to_search);
}

query command_handler::create_geo_query(coordinate location,
                                        polygon bounding_polygon,
                                        table table_to_search) {
    return query(table_to_search);
}

vector<row> run_query(query q, table table_to_search) { return vector<row>(); }

}  // namespace jt
