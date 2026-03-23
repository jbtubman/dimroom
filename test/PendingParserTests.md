# Pending Unit Tests for `parser.hpp`

List created by Claude Code, February 24, 2026.

Prompt was:

`Give a complete list of the unit tests required to test all public functions defined in the file @include/parser.hpp`

## `parser::parse_header(const string& header)`

| Test Name | Scenario | Status |
|---|---|---|
| `ParseHeaderParseSampleHeader` | Multi-column header, all fields trimmed, all types `undetermined` | **EXISTS** |
| `ParseHeaderEmptyString` | Empty string input → returns `unexpected(file_parse_error)` | **EXISTS** |
| `ParseHeaderTrimsWhitespace` | Column names with leading/trailing spaces | Missing |
| `ParseHeaderSingleColumn` | Input with only one column name | **EXISTS** |
| `ParseHeaderTwoColumns` | Input with two column names | **EXISTS** |
| `ParseHeaderWhitespaceOnly` | Input is only spaces/commas → graceful result or error | Missing |
| `ParseHeaderWindowsCRLFStrippedFromLastField` | Windows-style line endings: trim() removes \r and \n from all fields | **EXISTS** |
| `ParseHeaderUTF8BOMStrippedFromFirstField` | strips a leading UTF-8 BOM (\xEF\xBB\xBF) from any field | **EXISTS** |
| `ParseHeaderAllDataTypesUndetermined` | Every header field must have data type undetermined regardless of name | **EXISTS** |
| `StringUtilsRemoveUTF8ByteOrderMarkAndTrimWhiteSpace` | Make sure white space trimming works if there is a UTF-8 byte order mark | **EXISTS** |
| `StringUtilsRemoveUTF16BEByteOrderMarkAndTrimWhiteSpace` | Make sure white space trimming works if there is a big-endian UTF-16 byte order mark | **EXISTS** |
| `StringUtilsRemoveUTF16LEByteOrderMarkAndTrimWhiteSpace` | Make sure white space trimming works if there is a little-endian UTF-16 byte order mark | **EXISTS** |

---

## `parser::parse_data_row(const string& data_row)`

| Test Name | Scenario | Status |
|---|---|---|
| `ParseRowParseDataRow` | Mixed-type row (text, float, int, tags, empty fields) | **EXISTS** |
| `ParseRowIntegerFields` | Row containing only integers → all typed `integer` | **EXISTS** |
| `ParseRowFloatingFields` | Row containing only floats → all typed `floating` | **EXISTS** |
| `ParseRowBooleanFields` | Row with boolean values → typed `boolean` | **EXISTS** |
| `ParseRowGeoCoordinateField` | Row with a geo-coordinate value → typed `geo_coordinate` | **EXISTS** |
| `ParseRowTagsField` | Row with a `"""tag1, tag2"""` value → typed `tags` | **EXISTS** |
| `ParseRowQuotedFieldWithComma` | Quoted field containing a comma is kept as one field | **EXISTS** |
| `ParseRowAllEmptyFields` | Row of all empty values → all typed `undetermined` | **EXISTS** |
| `ParseRowSingleField` | Row with exactly one field | **EXISTS** |

---

## `parser::deduce_data_types_for_all_columns(const parser::header_and_data&)`

| Test Name | Scenario | Status |
|---|---|---|
| `GetDataTypeForAllColumns` | Full sample CSV → all 13 columns correctly typed | **EXISTS** |
| `DeduceTypesNoDataRows` | `all_data_fields` is empty → all columns remain `undetermined` | **EXISTS** |
| `DeduceTypesEarlyExit` | All types resolved on first data row → returns early | **EXISTS** |
| `DeduceTypesResolvedOnLaterRow` | First rows have empty fields, type resolved by a later row | **EXISTS** |
| `DeduceTypesWrongColumnCount` | A data row has fewer/more columns than the header → `unexpected(file_parse_error)` | **EXISTS** |
| `DeduceTypesInvalidMixedTypesCheckAllDataRows` | Same column has both integer and text values → `unexpected(file_parse_error)` | **EXISTS** |
| `DeduceTypesInvalidMixedTypesDoNotCheckAllDataRows` | Same column has both integer and text values but validity check is short-circuited → `e_cell_data_type::integer` | **EXISTS** |
| `DeduceTypesInvalidColumnReportedCheckAllDataRows` | Checks that the correct column number is reported in stderr on invalid type | **EXISTS** |
| `DeduceTypesInvalidColumnReportedDoNotCheckAllDataRows` | Ignores later incorrect data type because validity check is short-circuited. | **EXISTS** |

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
| `ParseFileParseLinesFromIfstream` | Opens valid file — **but does not check result or column types** | **EXISTS** |
| `ParseFileParseLinesIfstreamCorrectResult` | Valid stream → header fields and data rows match expected values | Missing |
| `ParseFileParseLinesIfstreamBadStream` | Bad/closed stream → `unexpected(file_empty_error)` | Missing |
| `ParseFileParseLinesIfstreamHeaderOnly` | Stream with only the header line → header parsed, empty data rows | Missing |
| `ParseFileParseLinesIfstreamColumnMismatch` | Data row has wrong column count → `unexpected(file_parse_error)` | Missing |

---

## Summary

| Status | Count |
|---|---|
| Already exist | 19 (one incomplete) |
| Missing | 8 |
| **Total required** | **27** |

The most critical gaps are: error path tests for all three `parse_lines` overloads, the
column-count mismatch and invalid-type error paths in `deduce_data_types_for_all_columns`,
and completing `ParseFileParseLinesFromIfstream` to actually verify the parsed result.
