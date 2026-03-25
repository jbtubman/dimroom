# Status and observations

This summarizes the state of things as of Version 2.27.

## Things Completed

As of March 24, 2026:

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
* UTF-8 printing in Windows has been solved.
* Running `test_dimroom` from the command line works correctly in any
  directory on Mac, Windows, and Android Linux.
* Tests can now be run in the `build` directory using the command `make test`.
* Several AI-generated unit tests were added for the function `parser::parse_header`.
  These are noted in the source.
* Some parsing functions had templated overloads added to allow them to accept
  "string-like" arguments like string views.
* All unit tests suggested for the file `parser.hpp` have been implemented.
* All unit tests suggested for the file `query.hpp` have been implemented.

## Things Remaining

* Continue improving documentation.
* Continue improving test coverage.
* Continue refactoring.
