#pragma once

// Rows of cells

#include <ranges>
#include <string>
#include <tuple>
#include <vector>

#include "cell.hpp"
#include "cell_types.hpp"
#include "parse_utils.hpp"
#include "parser.hpp"

namespace jt {
using std::string;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

class deprecated_row {
   private:
    vector<data_cell> container;

   public:
    auto& cells() { return container; }

    const auto& cells() const { return container; }
};
}  // namespace jt
