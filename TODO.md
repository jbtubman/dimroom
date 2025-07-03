# Status and observations

This summarizes the state of things before the code is submitted.

A lot of work still remains.

## Things remaining to be done.

As of July 3, 2025.

* Make all the tests do actual testing as opposed to running things for side-effects.
* Complete the construction of the CVS data rows.
* Implement the table class (columns plus rows).
* Implement querying on the rows.
* Implement querying on the whole table.
* Implement reading in the CSV file for parsing.
* Implement a user interface for querying for data.

## Observations

* Parsing the CSV file and determining the data types of the columns was a lot of work.
  * CSV parsing libraries that I found could not handle geographic coordinates and tags lists.
