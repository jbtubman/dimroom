# Status and observations

This summarizes the state of things as of Version 2.9.

Some work still remains.

## Things Completed

As of July 22, 2025:

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

## Things Remaining

* Implement numeric comparisons for greater and lesser, as well as for equals.
* Allow searching for multiple tags at once.
* Implement the ANDing of queries.
  * The internals of the queries use `std::ranges::views` for eventually
    implementing this efficiently.
