# Status and observations

## Things remaining to be done.

As of July 3, 2025.

* Make all the tests do actual testing as opposed to running things for side-effects.
* Complete the construction of the CVS data rows.
* Implement the table class (columns plus rows).
* Implement searching.
* Implement querying on the fields.
* Implement reading in the CSV file for parsing.
* Implement a user interface for querying for data.

## Observations

* Parsing the CSV file and determining the data types of the columns was a lot of work.
  * CSV parsing libraries that I found could not handle geographic coordinates and tags lists.
