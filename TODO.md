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

## Overhaul cell_types.hpp

There's a lot going on in this file.

Some things should be moved over to the parser. They do not belong here.

## Overhaul Parser

Need a parser class. DONE.

Need supporting classes: DONE.

(In cell_type?) The data_value_type variant, and the optional of that.

* header_field
  * name
  * e_cell_data_type (defaults to unknown).

* data_field
  * text
  * cell

## Overhaul Cells

Changed the class name to `data_cell`.

Do we really need to know the position of the cell?

We could determine the data type to associate with the column after all the data rows
have been read in. Then go row by row through the data until all the data types have been
deduced, or we run out of rows. DONE.

Have a new class - parsed_data_field. This holds the text and the guessed data type. DONE.

## Convert `header_and_data` to Table

That is basically done.

## parser::header_field and parser::data_field are the same!

Don't worry about it for now.

## NEXT STEP

Query stuff.
do a text field query first.

Given a field name, look up what type of values it holds.
Parse the query value argument accordingly.
Do we need a map that is the inverse of type_to_column_name_map? I think we will. DONE.
