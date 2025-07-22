# Status and observations

This summarizes the state of things as of Version 2.8.

Some work still remains.

## Things Completed

As of July 21, 2025:

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

## Things Remaining

* Implement the ANDing of queries.
  * The internals of the queries use `std::ranges::views` for eventually
    implementing this efficiently.
