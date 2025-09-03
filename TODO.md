# Status and observations

This summarizes the state of things as of Version 2.18.

## Things Completed

As of September 2, 2025:

* Reads in CSV file.
* Parses the header and data rows.
* Deduces the probable data types of each column, where there is sufficient data.
* Much more comprehensive test coverage.
* Simple geographic queries now work.
* Coordinate within polygon queries now work.
* Text queries work.
* Integer queries work.
* Floating point queries work.
* Boolean queries work.
* Tags queries work.
* Renamed some classes and aliases to better match their real meaning.
* Refactored query dispatching from command_interpreter class to query class.
* No longer crashes if incorrect data types are used in queries.
* Implemented numeric comparisons for `=`, `!=`, `>`, `>=`, `<`, and `<=`.
* Updated help text.
* ANDed queries work.
* Unit testing library changed to [Google Test](https://google.github.io/googletest/).
* Byte-order mark bug on Windows fixed.

## Things Remaining

* Fix bug in printing of degree signs on output in Windows.
* Continue improving documentation.
* Continue improving test coverage.
* Continue refactoring.
