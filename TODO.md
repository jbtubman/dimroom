# Status and observations

This summarizes the state of things before the code is submitted.

A lot of work still remains.

## Things Completed

As of July 13, 2025:

* Reads in CSV file.
* Parses the header and data rows.
* Deduces the probable data types of each column, where there is sufficient data.


## Things Remaining

* Complete the test coverage.
* Rename some of the classes and data types to be less confusing.
* Implement tag queries.
* Fix geographic coordinate queries.
  * The code that works fine for parsing coordinates when the CSV file
    is read in, does not work as well when parsing queries.
* Implement "within polygon" queries. (The code for determining that is
  present, but not hooked up to the user interface.)
* Implement the ANDing of queries.
  * The internals of the queries use `std::ranges::views` for eventually
    implementing this efficiently.


## Observations

* Parsing the CSV file and determining the data types of the columns was a lot of work.
  * CSV parsing libraries that I found could not handle geographic coordinates and tags lists.
