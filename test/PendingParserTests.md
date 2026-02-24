# Pending Unit Tests for `parser.hpp`

List created by Claude Code, February 24, 2026.

## `parser::parse_header(const string& header)`

| Test Name | Scenario | Status |
|---|---|---|
| `ParseHeaderParseSampleHeader` | Multi-column header, all fields trimmed, all types `undetermined` | **EXISTS** |
| `ParseHeaderSingleColumn` | Input with only one column name | Missing |
| `ParseHeaderTrimsWhitespace` | Column names with leading/trailing spaces | Missing |
| `ParseHeaderEmptyString` | Empty string input → returns `unexpected(file_parse_error)` | Missing |
| `ParseHeaderWhitespaceOnly` | Input is only spaces/commas → graceful result or error | Missing |

---

## `parser::parse_data_row(const string& data_row)`

| Test Name | Scenario | Status |
|---|---|---|
| `ParseRowParseDataRow` | Mixed-type row (text, float, int, tags, empty fields) | **EXISTS** |
| `ParseRowIntegerFields` | Row containing only integers → all typed `integer` | Missing |
| `ParseRowFloatingFields` | Row containing only floats → all typed `floating` | Missing |
| `ParseRowBooleanFields` | Row with boolean values → typed `boolean` | Missing |
| `ParseRowGeoCoordinateField` | Row with a geo-coordinate value → typed `geo_coordinate` | Missing |
| `ParseRowTagsField` | Row with a `"""tag1, tag2"""` value → typed `tags` | Missing |
| `ParseRowQuotedFieldWithComma` | Quoted field containing a comma is kept as one field | Missing |
| `ParseRowAllEmptyFields` | Row of all empty values → all typed `undetermined` | Missing |
| `ParseRowSingleField` | Row with exactly one field | Missing |

---

## `parser::deduce_data_types_for_all_columns(const parser::header_and_data&)`

| Test Name | Scenario | Status |
|---|---|---|
| `GetDataTypeForAllColumns` | Full sample CSV → all 13 columns correctly typed | **EXISTS** |
| `DeduceTypesNoDataRows` | `all_data_fields` is empty → all columns remain `undetermined` | Missing |
| `DeduceTypesEarlyExit` | All types resolved on first data row → returns early | Missing |
| `DeduceTypesResolvedOnLaterRow` | First rows have empty fields, type resolved by a later row | Missing |
| `DeduceTypesWrongColumnCount` | A data row has fewer/more columns than the header → `unexpected(file_parse_error)` | Missing |
| `DeduceTypesInvalidMixedTypes` | Same column has both integer and text values → `unexpected(file_parse_error)` | Missing |
| `DeduceTypesInvalidColumnReported` | Checks that the correct column number is reported in stderr on invalid type | Missing |

---

## `parse_lines(const VectorString& input_lines)` and `parse_lines(VectorString&&)`

| Test Name | Scenario | Status |
|---|---|---|
| `ParseFileParseLines` | Happy path const lvalue ref → correct header and row count | **EXISTS** |
| `ParseFileParseLinesRvalue` | Happy path rvalue (moved) vector → same result as lvalue | Missing |
| `ParseFileParseLinesEmptyVector` | Empty vector → `unexpected(file_empty_error)` | Missing |
| `ParseFileParseLinesHeaderOnly` | Vector with only the header row → header parsed, no data rows | Missing |
| `ParseFileParseLinesColumnCountMismatch` | Data row has wrong column count → `unexpected(file_parse_error)` | Missing |
| `ParseFileParseLinesUnparsableDataRow` | Unparseable data row → `unexpected(file_parse_error)` | Missing |

---

## `parse_lines(std::ifstream& instream)`

| Test Name | Scenario | Status |
|---|---|---|
| `ParseFileParseLinesFromIfstream` | Opens valid file — **but does not check result or column types** | **EXISTS (incomplete)** |
| `ParseFileParseLinesIfstreamCorrectResult` | Valid stream → header fields and data rows match expected values | Missing |
| `ParseFileParseLinesIfstreamBadStream` | Bad/closed stream → `unexpected(file_empty_error)` | Missing |
| `ParseFileParseLinesIfstreamHeaderOnly` | Stream with only the header line → header parsed, empty data rows | Missing |
| `ParseFileParseLinesIfstreamColumnMismatch` | Data row has wrong column count → `unexpected(file_parse_error)` | Missing |

---

## Summary

| Status | Count |
|---|---|
| Already exist | 5 (one incomplete) |
| Missing | 22 |
| **Total required** | **27** |

The most critical gaps are: error path tests for all three `parse_lines` overloads, the
column-count mismatch and invalid-type error paths in `deduce_data_types_for_all_columns`,
and completing `ParseFileParseLinesFromIfstream` to actually verify the parsed result.
