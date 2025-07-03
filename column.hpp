#pragma once

// Defines columns and their properties.

#include <map>
#include <string>

#include "cell_types.hpp"

namespace jt {
using std::string;

class column {
   public:
    string name{};
    std::size_t position{};
    cell_type column_type{};

    column(const string& nm, std::size_t pos, cell_type ctype = {})
        : name(nm), position(pos), column_type{ctype} {}

    column() = default;
    column(const column&) = default;
    column(column&&) = default;
    ~column() = default;

    void swap(column& other) noexcept {
        using std::swap;
        swap(name, other.name);
        swap(position, other.position);
        swap(column_type, other.column_type);
    }

    column& operator=(const column& other) noexcept {
        column tmp{other};
        swap(tmp);
        return *this;
    }

    bool operator==(const column& other) const {
        return (position == other.position) &&
               (column_type == other.column_type) && (name == other.name);
    }

    column& operator=(column&& other) noexcept {
        column tmp{std::move(other)};
        swap(tmp);
        return *this;
    }
};

// inline bool operator==(const column& lhs, const column& rhs) {
//   return lhs.operator==(rhs);
// }

class columns {
   public:
    std::vector<column> cols;
    // Allows quick acess to column information.
    std::map<string, column> name_to_column{};
    using mapped_type = std::map<string, column>::mapped_type;

    columns& add(column& col) {
        cols.push_back(col);
        name_to_column.insert({col.name, col});
        return *this;
    }

    auto find(const string& field_name) -> std::optional<mapped_type> {
        auto it = name_to_column.find(field_name);
        if (it == name_to_column.end()) {
            return {};
        }
        return it->second;
    }
};

}  // namespace jt
