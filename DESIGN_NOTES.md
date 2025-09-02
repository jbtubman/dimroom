# DESIGN NOTES

Jim Tubman
September 2025

## Introduction

I had a lot of fun doing this. It gave me an opportunity to learn a lot
about the many powerful features that have been incorporated into the
C++ language and libraries over the last ten years. Having ranges, views,
and monadic classes (like `optional` and `variant`) available made it
easier to implement certain features.

## Design Decisions

The original requirements are in the document [ProblemStatement.md](./ProblemStatement.md).

Parsing the input file and the command line inputs took considerable effort.
The nature of the geographic coordinate field type (with two different
representations) and the tags field type meant that a simple splitting of lines
at the commas would not do. Every data line required three passes to do a rough
split, a field-by-field recombining, then a correct split. Many regular expressions
were used. (The site [regex101.com](regex101.com) was a great help, but it didn't
solve all the problems.)

This implementation goes beyond the original requirements by adding the
ability to deduce the data type of each column. Deducing the data types of the
columns was an interesting challenge. I used a combination of regular expressions
and attempted type conversions inside `try` blocks to do it. I'm proud of the
`operator||` implementation I devised to help with that process.

## Things I Would Do Differently

If I were to do it again, I would come up with a more consistent naming convention
for data types and classes from the beginning. Names were made more consistent
in later code refactoring.

For a true commercial project, I would use a relational database as a backend.
That would make it much easier to represent null values, geographic coordinates,
and tags. Querying of large data sets would be much faster because indexing would
be available.

A commercial product would need a far more extensive test suite.

For commercial use, I would have used third party libraries for things like
parsing.
