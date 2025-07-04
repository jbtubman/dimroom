#include "../cell.hpp"

#include <algorithm>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "CppUnitTestFramework.hpp"
#include "../cell_types.hpp"
#include "general_fixture.hpp"

namespace {
using std::string;
using namespace jt;

struct MyFixture : general_fixture {
    // ...

};
}  // namespace

using std::println;
using std::regex;
using std::string;
using std::string_view;
using std::vector;

namespace ranges = std::ranges;
namespace views = std::views;

using namespace jt;

// TODO: Surely there is something in the STL to do this.
template <class Collection>
auto find_final(Collection&& collection) {
    auto col = std::forward<Collection>(collection);
    auto current = col.begin();
    auto next = current;
    next++;
    while (next != col.end()) {
        current++;
        next++;
    }
    return current;
}

// TODO: proper tests in cell_test.cpp.

TEST_CASE(MyFixture, Cell) {
    SECTION("splitting") {
        using std::operator""sv;

        string_view sample_row_sv{MyFixture::sample_row_3};

        auto fields_view = sample_row_sv | views::split(","sv) |
                           views::transform([](auto field_sv) {
                               return string(field_sv.begin(), field_sv.end());
                           });

        auto raw_field_strings = fields_view | ranges::to<vector>();

        for (auto s : raw_field_strings) {
            println(stderr, "\"{}\", ", s);
        }

        // Glue together the tags fields.
        regex start_tag_rx{R"(^""".*)"};
        auto start_tag_fn = [start_tag_rx](const string& s) {
            return regex_match(s, start_tag_rx);
        };

        regex end_tag_rx{R"(.*"""$)"};
        auto end_tag_fn = [end_tag_rx](const string& s) {
            return regex_match(s, end_tag_rx);
        };
        auto beginning_tags = std::find_if(
            raw_field_strings.begin(), raw_field_strings.end(), start_tag_fn);

        if (beginning_tags != raw_field_strings.end()) {
            // Look for the end.
            auto start_tags = beginning_tags;
            ++start_tags;  // make sure it isn't the end.
            auto ending_tags =
                std::find_if(start_tags, raw_field_strings.end(), end_tag_fn);

            if (ending_tags != raw_field_strings.end()) {
                // glue them together.
                auto the_end = ending_tags;
                ++the_end;
                vector<string> tag_vec(beginning_tags, the_end);
                println(stderr, "tag_vec: {}", tag_vec);

                string initial{""};
                auto res = ranges::fold_left(tag_vec, initial,
                                             [](string acc, string s) {
                                                 if (acc.empty()) {
                                                     return s;
                                                 } else {
                                                     return acc + "," + s;
                                                 }
                                             });
                println(stderr, "res: {}", res);
            }
        }
    }
}
