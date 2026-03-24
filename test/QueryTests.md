# Unit Tests for `query.hpp`

List created by Claude Code, March 24, 2026.

Prompt was:

`Give a complete list of the unit tests required to test all public functions defined in the file @include/query.hpp`

---

## Constructor `query(table& tb, const string& col_name, comparison comp_)`

| Test Name | Scenario | Status |
|---|---|---|
| `QueryConstructorDefaultComparison` | Constructed with default `comp` → `comp` is `equal_to` | **EXISTS** |
| `QueryConstructorExplicitComparison` | Constructed with explicit comparison (e.g., `greater`) → `comp` is set correctly | **EXISTS** |
| `QueryConstructorStoresColumnName` | `column_name` member matches the string passed in | **EXISTS** |
| `QueryConstructorStoresTableRef` | `t` member refers to the table passed in | **EXISTS** |

---

## `execute(const string& query_value_s)`

| Test Name | Scenario | Status |
|---|---|---|
| `ExecuteDispatchesTextColumn` | Column is `text` type → dispatches to `string_match`, returns matching rows | **EXISTS** |
| `ExecuteDispatchesIntegerColumn` | Column is `integer` type → dispatches to `integer_match`, returns matching rows | **EXISTS** |
| `ExecuteDispatchesBooleanColumnTrue` | Column is `boolean`, value is `"true"` → dispatches to `boolean_match`, returns matching rows | **EXISTS** |
| `ExecuteDispatchesBooleanColumnFalse` | Column is `boolean`, value is `"false"` → dispatches to `boolean_match`, returns matching rows | **EXISTS** |
| `ExecuteDispatchesFloatingColumn` | Column is `floating` type → dispatches to `floating_match`, returns matching rows | **EXISTS** |
| `ExecuteDispatchesGeoCoordinateColumn` | Column is `geo_coordinate` type → dispatches to `geo_coordinate_match`, returns matching rows | **EXISTS** |
| `ExecuteDispatchesTagsColumn` | Column is `tags` type → dispatches to `tags_match`, returns matching rows | **EXISTS** |
| `ExecuteInvalidColumnName` | Column name does not exist in the table → returns empty rows, prints error to stderr | **EXISTS** |
| `ExecuteUnsupportedColumnType` | Column exists but has `undetermined` or `invalid` type → returns empty rows, prints error to stderr | **EXISTS** |
| `ExecuteInvalidBooleanInput` | Column is `boolean`, value is not parseable as bool → returns empty rows, prints error to stderr | **EXISTS** |
| `ExecuteInvalidFloatingInput` | Column is `floating`, value is not parseable as float → returns empty rows, prints error to stderr | **EXISTS** |
| `ExecuteInvalidGeoCoordinateInput` | Column is `geo_coordinate`, value is not parseable as a coordinate → returns empty rows, prints error to stderr | **EXISTS** |
| `ExecuteInvalidIntegerInput` | Column is `integer`, value is not parseable as int → returns empty rows, prints error to stderr | **EXISTS** |

---

## `string_match(const string& query_value, table::opt_rows rows_to_query)`

| Test Name | Scenario | Status |
|---|---|---|
| `TextTestStringSearch1` | Equal-to: unique match in text column → 1 result | **EXISTS** |
| `TextTestStringSearch2` | Equal-to: multiple matches in text column → N results | **EXISTS** |
| `TextTestStringSearchEmpty` | Equal-to: no match → empty result | **EXISTS** |
| `StringMatchNotEqualTo` | `not_equal_to`: returns all rows where the cell differs from the query value | **EXISTS** |
| `StringMatchGreater` | `greater`: returns rows where cell value sorts after the query value | **EXISTS** |
| `StringMatchLess` | `less`: returns rows where cell value sorts before the query value | **EXISTS** |
| `StringMatchGreaterEqual` | `greater_equal`: returns rows where cell value sorts at or after the query value | **EXISTS** |
| `StringMatchLessEqual` | `less_equal`: returns rows where cell value sorts at or before the query value | **EXISTS** |
| `StringMatchWithRowsToQuery` | `rows_to_query` is provided → searches only that subset, not all table rows | **EXISTS** |
| `StringMatchQuotedInput` | Query value has surrounding quotes → dequoted before comparison | **EXISTS** |
| `StringMatchEmptyQueryValue` | Query value is `""` → matches cells that are empty | **EXISTS** |
| `StringMatchInvalidColumnName` | `column_name` not in table → returns empty results | **EXISTS** |

---

## `integer_match(const string& query_value, table::opt_rows rows_to_query)` (string overload)

| Test Name | Scenario | Status |
|---|---|---|
| `IntegerMatchStringOverloadMatch` | Valid integer string → same result as calling the `int` overload directly | **EXISTS** |
| `IntegerMatchStringOverloadNoMatch` | Valid integer string for a value not in the column → empty result | **EXISTS** |
| `IntegerMatchStringOverloadWithRowsToQuery` | `rows_to_query` provided → searches only that subset | **EXISTS** |

---

## `integer_match(int query_value, table::opt_rows rows_to_query)` (int overload)

| Test Name | Scenario | Status |
|---|---|---|
| `IntegerTestIntegerSearch0` | Equal-to: value not in table → empty result | **EXISTS** |
| `IntegerTestIntegerSearch1` | Equal-to: value present once → 1 result | **EXISTS** |
| `IntegerTestIntegerSearch4` | Equal-to: value present multiple times → N results | **EXISTS** |
| `IntegerMatchNotEqualTo` | `not_equal_to`: returns all rows whose cell differs from the query value; empty cells are considered not equal | **EXISTS** |
| `IntegerMatchGreater` | `greater`: returns rows where cell value > query value | **EXISTS** |
| `IntegerMatchLess` | `less`: returns rows where cell value < query value | **EXISTS** |
| `IntegerMatchGreaterEqual` | `greater_equal`: returns rows where cell value >= query value | **EXISTS** |
| `IntegerMatchLessEqual` | `less_equal`: returns rows where cell value <= query value | **EXISTS** |
| `IntegerMatchEmptyCellEqualTo` | Row has an empty cell in the integer column → not returned for `equal_to` | **EXISTS** |
| `IntegerMatchEmptyCellNotEqualTo` | Row has an empty cell in the integer column → returned for `not_equal_to` | **EXISTS** |
| `IntegerMatchWithRowsToQuery` | `rows_to_query` provided → searches only that subset | **EXISTS** |
| `IntegerMatchInvalidColumnName` | `column_name` not in table → returns empty results | **EXISTS** |

---

## `boolean_match(bool query_value, table::opt_rows rows_to_query)` (bool overload)

| Test Name | Scenario | Status |
|---|---|---|
| `BooleanMatchTrueEqualTo` | `equal_to`, query `true` → returns only rows with `true` in the column | **EXISTS** |
| `BooleanMatchFalseEqualTo` | `equal_to`, query `false` → returns rows with `false` and rows with empty cells (empty == false) | **EXISTS** |
| `BooleanMatchTrueNotEqualTo` | `not_equal_to`, query `true` → returns rows with `false` and empty cells | **EXISTS** |
| `BooleanMatchFalseNotEqualTo` | `not_equal_to`, query `false` → returns rows with `true` | **EXISTS** |
| `BooleanMatchGreater` | `greater`, query `false` → returns rows with `true` (true > false) | **EXISTS** |
| `BooleanMatchGreaterTrueReturnsNothing` | `greater`, query `true` → nothing is greater than `true` → empty result | **EXISTS** |
| `BooleanMatchLess` | `less`, query `true` → returns rows with `false` and empty cells (false < true) | **EXISTS** |
| `BooleanMatchLessFalseReturnsNothing` | `less`, query `false` → nothing is less than `false` → empty result | **EXISTS** |
| `BooleanMatchGreaterEqual` | `greater_equal`, query `false` → returns all rows (everything >= false) | **EXISTS** |
| `BooleanMatchLessEqual` | `less_equal`, query `true` → returns all rows (everything <= true) | **EXISTS** |
| `BooleanMatchEmptyCellCountedAsFalse` | Empty cell in boolean column counts as `false` for comparison purposes | **EXISTS** |
| `BooleanMatchWithRowsToQuery` | `rows_to_query` provided → searches only that subset | **EXISTS** |

---

## `boolean_match(const string& query_value, table::opt_rows rows_to_query)` (string overload)

| Test Name | Scenario | Status |
|---|---|---|
| `BooleanMatchStringTrue` | `"true"` → same result as calling `bool` overload with `true` | **EXISTS** |
| `BooleanMatchStringFalse` | `"false"` → same result as calling `bool` overload with `false` | **EXISTS** |
| `BooleanMatchStringYes` | `"yes"` → treated as `true` | **EXISTS** |
| `BooleanMatchStringNo` | `"no"` → treated as `false` | **EXISTS** |
| `BooleanMatchStringYesCap` | `"Yes"` → treated as `true` | **EXISTS** |
| `BooleanMatchStringNoCap` | `"No"` → treated as `false` | **EXISTS** |
| `BooleanMatchStringOne` | `"1"` → treated as `true` | **EXISTS** |
| `BooleanMatchStringZero` | `"0"` → treated as `false` | **EXISTS** |
| `BooleanMatchStringInvalid` | Non-boolean string (e.g., `"maybe"`) → returns empty `table::rows` | **EXISTS** |
| `BooleanMatchStringWithRowsToQuery` | `rows_to_query` provided → searches only that subset | **EXISTS** |

---

## `floating_match(float query_value, table::opt_rows rows_to_query)` (float overload)

| Test Name | Scenario | Status |
|---|---|---|
| `FloatingMatchEqualToMatch` | `equal_to`: value present in column (within `is_close` tolerance) → returns matching rows | **EXISTS** |
| `FloatingMatchEqualToNoMatch` | `equal_to`: value not present → empty result | **EXISTS** |
| `FloatingMatchNotEqualTo` | `not_equal_to`: returns all rows where cell is not close to query value; empty cells are included | **EXISTS** |
| `FloatingMatchGreater` | `greater`: returns rows where cell value > query value | **EXISTS** |
| `FloatingMatchLess` | `less`: returns rows where cell value < query value | **EXISTS** |
| `FloatingMatchGreaterEqual` | `greater_equal`: returns rows where cell value >= query value or is close to it | **EXISTS** |
| `FloatingMatchLessEqual` | `less_equal`: returns rows where cell value <= query value or is close to it | **EXISTS** |
| `FloatingMatchEmptyCellEqualTo` | Empty cell in float column → not returned for `equal_to` | **EXISTS** |
| `FloatingMatchEmptyCellNotEqualTo` | Empty cell in float column → returned for `not_equal_to` | **EXISTS** |
| `FloatingMatchWithRowsToQuery` | `rows_to_query` provided → searches only that subset | **EXISTS** |
| `FloatingMatchInvalidColumnName` | `column_name` not in table → returns empty results | **EXISTS** |

---

## `floating_match(const string& query_value, table::opt_rows rows_to_query)` (string overload)

| Test Name | Scenario | Status |
|---|---|---|
| `FloatingMatchStringOverloadMatch` | Valid float string → same result as calling `float` overload directly | **EXISTS** |
| `FloatingMatchStringOverloadNoMatch` | Valid float string for a value not in the column → empty result | **EXISTS** |
| `FloatingMatchStringOverloadWithRowsToQuery` | `rows_to_query` provided → searches only that subset | **EXISTS** |

---

## `geo_coordinate_match(const coordinate& coord, table::opt_rows rows_to_query)` (coordinate overload)

| Test Name | Scenario | Status |
|---|---|---|
| `GeoCoordinateMatchDecimalMatch` | Decimal-format coordinate matching a row → returns that row | **EXISTS** |
| `GeoCoordinateMatchDegMinMatch` | Degrees/minutes coordinate matching a row → returns that row | **EXISTS** |
| `GeoCoordinateMatchNoMatch` | Coordinate not present in column → empty result | **EXISTS** |
| `GeoCoordinateMatchEmptyCell` | Row has empty cell in geo coordinate column → not returned | **EXISTS** |
| `GeoCoordinateMatchWithRowsToQuery` | `rows_to_query` provided → searches only that subset | **EXISTS** |
| `GeoCoordinateMatchInvalidColumnName` | `column_name` not in table → returns empty results | **EXISTS** |

---

## `geo_coordinate_match(const string& coord, table::opt_rows rows_to_query)` (string overload)

| Test Name | Scenario | Status |
|---|---|---|
| `GeoCoordinateMatchStringDecimal` | Valid decimal coordinate string → same result as `coordinate` overload | **EXISTS** |
| `GeoCoordinateMatchStringDegMin` | Valid degrees/minutes coordinate string → same result as `coordinate` overload | **EXISTS** |
| `GeoCoordinateMatchStringNoMatch` | Valid coordinate string for a location not in the table → empty result | **EXISTS** |
| `GeoCoordinateMatchStringWithRowsToQuery` | `rows_to_query` provided → searches only that subset | **EXISTS** |

---

## `tags_match(const string& tags_string, table::opt_rows rows_to_query)` (string overload)

| Test Name | Scenario | Status |
|---|---|---|
| `TagsMatchStringSingleTagMatch` | Single tag string → returns all rows containing that tag | **EXISTS** |
| `TagsMatchStringMultipleTagsMatch` | Comma-separated tags → returns rows containing any one of the tags | **EXISTS** |
| `TagsMatchStringIrregularSpacing` | Tags separated by `, ` with varied spacing → spacing normalized before matching | **EXISTS** |
| `TagsMatchStringQuotedTags` | Tags with surrounding quotes → dequoted before matching | **EXISTS** |
| `TagsMatchStringNoMatch` | Tag not present in any row → empty result | **EXISTS** |
| `TagsMatchStringWithRowsToQuery` | `rows_to_query` provided → searches only that subset | **EXISTS** |

---

## `tags_match(const vector<string>& tags, table::opt_rows rows_to_query)` (vector overload)

| Test Name | Scenario | Status |
|---|---|---|
| `TagsMatchVectorSingleTagMatch` | Vector with one tag → returns rows containing that tag | **EXISTS** |
| `TagsMatchVectorMultipleTagsAnyMatch` | Vector with multiple tags → returns rows containing any one of them (OR semantics) | **EXISTS** |
| `TagsMatchVectorEmptyVector` | Empty vector → no rows match → empty result | **EXISTS** |
| `TagsMatchVectorNoMatch` | Tags not present in any row → empty result | **EXISTS** |
| `TagsMatchVectorEmptyCell` | Row has empty cell in tags column → not returned | **EXISTS** |
| `TagsMatchVectorWithRowsToQuery` | `rows_to_query` provided → searches only that subset | **EXISTS** |
| `TagsMatchVectorInvalidColumnName` | `column_name` not in table → returns empty results | **EXISTS** |

---

## `point_in_polygon_match(const polygon_t& polygn, table::opt_rows rows_to_query)`

| Test Name | Scenario | Status |
|---|---|---|
| `PointInPolygonMatchInsidePolygon` | Coordinate in table is inside the polygon → that row is returned | **EXISTS** |
| `PointInPolygonMatchOutsidePolygon` | Coordinate in table is outside the polygon → not returned | **EXISTS** |
| `PointInPolygonMatchOnEdge` | Coordinate lies exactly on the polygon boundary → behavior tested (implementation-defined) | **EXISTS** |
| `PointInPolygonMatchMultipleMatches` | Multiple rows have coordinates inside the polygon → all returned | **EXISTS** |
| `PointInPolygonMatchNoMatch` | No coordinates fall inside the polygon → empty result | **EXISTS** |
| `PointInPolygonMatchEmptyCell` | Row has empty cell in geo coordinate column → not returned | **EXISTS** |
| `PointInPolygonMatchWithRowsToQuery` | `rows_to_query` provided → searches only that subset | **EXISTS** |
| `PointInPolygonMatchInvalidColumnName` | `column_name` not in table → returns empty results | **EXISTS** |

---

## Summary

| Status | Count |
|---|---|
| Already exist | 111 |
| Missing | 0 |
| **Total required** | **111** |
