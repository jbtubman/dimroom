#include "parser.hpp"

namespace jt {
    using std::vector;
    using std::string;

// static inline parser::data_fields get_data_fields_for_one_column(
//     const parser::all_data_fields& all_df, size_t column_index) {
//     return ranges::fold_left(all_df, parser::data_fields{},
//                              [column_index](auto&& acc, auto&& v) {
//                                  return shove_back(acc, v[column_index]);
//                              });
// }

// static inline e_cell_data_type get_data_type_for_column(
//     const parser::data_fields& dfs_for_one_column) {
//     return ranges::fold_left(
//         dfs_for_one_column, e_cell_data_type::undetermined,
//         [](e_cell_data_type acc, const parser::data_field& df) {
//             return acc || df.data_type;
//         });
// }

// template <typename VecString>
// static inline parser::header_and_data _parse_lines(VecString&& input_lines) {
//     auto in_lines = std::move<VecString>(input_lines);
//     auto len = in_lines.size();
//     auto first = in_lines.begin();
//     auto last = in_lines.end();
//     if (first == last) {
//         return header_and_data();
//     }

//     auto second = ++first;
//     if (second == last) {
//         println(stderr, "No data rows");
//         return header_and_data(parse_header(in_lines[0]));
//     } else {
//         println(stderr, "{} data rows", last - second);
//     }

//     header_and_data result(parse_header(in_lines[0]));
//     auto data_range = ranges::subrange(second, last);

//     for (auto current = second; current != last; ++current) {
//         println(stderr, "data row: \"{}\"", *current);
//         const string& s = *current;
//         data_fields dfs = parse_data_row(s);
//         result.data_fields_vec_.push_back(dfs);
//         println(stderr, "result.data_fields_vec_.size(): {}",
//                 result.data_fields_vec_.size());
//     }

//     const vector<e_cell_data_type> cell_data_types_vec =
//         parser::get_data_types_for_all_columns(result.data_fields_vec_);

//     // Add the cell data info to the headers.
//     auto zipped = ranges::zip_view(result.header_fields_, cell_data_types_vec);
//     parser::header_fields zz = ranges::transform(
//         zipped, [](std::pair<header_field, e_cell_data_type>& hf_ct) {
//             auto& hf = hf_ct.first;
//             hf.data_type = hf_ct.second;
//             return hf;
//         });

//     return result;
// }

parser::header_and_data parse_lines(
    const vector<string>& input_lines) {
    return _parse_lines<const vector<string>&>(input_lines);
}

parser::header_and_data parse_lines(
    vector<string>&& input_lines) {
    return _parse_lines(input_lines);
}

}  // namespace jt
