# Status and observations

This summarizes the state of things before the code is submitted.

Some work still remains.

## Things Completed

As of July 18, 2025:

* Reads in CSV file.
* Parses the header and data rows.
* Deduces the probable data types of each column, where there is sufficient data.
* Much more comprehensive test coverage.
* Simple geographic queries now work.
* Text queries work.
* Integer queries work.
* Floating point queries work.
* Boolean queries work.

## Things Remaining

* Rename some of the classes and data types to be less confusing.
* Implement tag queries.
* Implement "within polygon" queries. (The code for determining that is
  present, but not hooked up to the user interface.)
* Implement the ANDing of queries.
  * The internals of the queries use `std::ranges::views` for eventually
    implementing this efficiently.
