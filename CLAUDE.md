# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Dimroom** is a read-only command-line query tool for photo metadata stored in CSV files — a simplified, terminal-based Lightroom alternative. It parses a CSV file, auto-detects column data types, and lets users run queries against the data interactively.

## Build Commands

The build directory must exist and be configured before building. From the project root:

```bash
mkdir -p build && cd build
cmake ..        # configure (downloads googletest via FetchContent)
make            # build both dimroom and test_dimroom
make test       # run all tests
```

Run the program:
```bash
cd build
./dimroom ../test/data/sample.csv
```

Run a specific test binary directly (for verbose output):
```bash
cd build
./test/test_dimroom --gtest_filter=SomeTestSuite.SomeTest
```

## Architecture

All source code is C++23. The `namespace jt` wraps all project types.

### Data Flow

1. `src/dimroom.cpp` — entry point; uses `command_line` to get the filename, `command_handler` to read the file, then passes the resulting `table` to `read_eval_print`.
2. `src/command_line.cpp` / `include/command_line.hpp` — REPL loop; dispatches `describe`, `query`, `help`, `exit`/`quit` commands.
3. `src/query.cpp` / `include/query.hpp` — implements querying logic for all column types.
4. `include/parser.hpp` — stateless `parser` class (all static methods); parses header and data rows from CSV; deduces column types via `deduce_data_types_for_all_columns`.
5. `include/table.hpp` — `table` class; holds `header_fields_t` and `rows`; constructed via `table::make_table_from_file`.
6. `include/cell.hpp`, `include/cell_types.hpp` — `cell_value_types` is a `std::variant<monostate, monostate, float, bool, int, string, coordinate, vector<string>>` representing the seven supported data types.
7. `include/coordinates.hpp` — `coordinate` type; supports two geographic formats: decimal (`51.05011, -114.08529`) and degrees/minutes (`36° 00' N, 138° 00' E`).
8. `include/parse_utils.hpp` — CSV field splitting utilities, including `fix_quoted_fields` which handles the multi-pass splitting needed for complex fields.

### Key Design Patterns

- **`std::expected`** is used pervasively for error propagation instead of exceptions.
- **`e_cell_data_type` with `operator||`** — a custom "type-OR" operator merges column type deductions across rows (undetermined + T = T; mismatched types = invalid).
- **Formatter pattern** — formatter specializations for custom types are conditionally included at the bottom of their primary headers via `#define`/`#include`/`#undef` guards (e.g., `TABLE_INCLUDE_FORMATTER`, `CELL_TYPES_INCLUDE_FORMATTER`, `PARSER_INCLUDE_FORMATTER`).
- **Windows vs. Mac/Linux** — `command_handler` uses `wifstream` on Windows (`_WIN64`) and `ifstream` elsewhere. On Mac/Linux, `readline` is linked for interactive input; Windows falls back to standard I/O.

### Supported Column Data Types

`undetermined`, `invalid`, `floating`, `boolean`, `integer`, `text`, `geo_coordinate`, `tags`

Boolean values accept: `true`/`false`, `yes`/`no`, `Yes`/`No`, `1`/`0`.

### Test Structure

Tests live in `test/src/test_dimroom.cpp` (the single test binary entry point), with individual test suites in `test/include/*_test.hpp`. Test data CSV files are in `test/data/`.

### CMake Notes

- Requires CMake 4.2+ and a C++23-capable compiler (Clang 17+ on Mac, MSVC 19.44+ on Windows).
- `include/dimroomConfig.h` is generated from `include/dimroomConfig.h.in` with version macros.
- `cmake/FindReadline.cmake` locates the readline library; the build adapts if it is absent (Windows).
- GoogleTest is fetched via `FetchContent` at configure time (tag `v1.17.0`).
