# Copyright 2025, James B. Tubman
CPP = /usr/bin/c++

CFLAGS = -std=c++23

# TEST_INCLUDE_DIR = -I../CppUnitTestFramework

.SUFFIXES : .o .cpp

OBJS = main.o parser.o

# TEST_OBJS = test/*.o

.cpp.o :
	$(CPP) $(CFLAGS) -c $<

geologic: $(OBJS)
	$(CPP) -std=c++23 -o $@ $(OBJS)

.PHONY: test/test
test/test: test/main.cpp parser.o parser.hpp parser.cpp cell_types.hpp column.hpp jt_concepts.hpp parse_utils.hpp utility.hpp
	cd test ; make test
#	$(CPP) -std=c++23 -I../CppUnitTestFramework $(?) -o test/test

.PHONY: do_test
do_test: test/test
	./test/test

cell.hpp: cell_types.hpp column.hpp coordinates.hpp parser.hpp

cell_types.hpp: coordinates.hpp type_utility.hpp utility.hpp

column.hpp: cell_types.hpp

command_handler.hpp: coordinates.hpp query.hpp table.hpp

command_interpreter.hpp: command_handler.hpp query.hpp table.hpp

# no in-project dependencies
contains.hpp:

coordinates_formatter.hpp: coordinates.hpp

# no in-project dependencies
coordinates.hpp:

# no in-project dependencies
file_io.hpp:

parse_utils.hpp: cell_types.hpp coordinates.hpp jt_concepts.hpp utility.hpp

parser.o: parser.cpp
#	$(CPP) -std=c++23 -c $(?)

parser.cpp: parser.hpp

parser.hpp: cell_types.hpp column.hpp jt_concepts.hpp parse_utils.hpp utility.hpp

# no in-project dependencies
query.hpp:

row.hpp: cell.hpp cell_types.hpp parse_utils.hpp parser.hpp vectorize.hpp

table.hpp: column.hpp cell.hpp row.hpp parser.hpp

# no in-project dependencies
type_utility.hpp:

utility.hpp: jt_concepts.hpp

# no in-project dependencies
vectorize.hpp:

main.cpp: cell_types.hpp command_interpreter.hpp contains.hpp coordinates_formatter.hpp coordinates.hpp table.hpp

clean:
	/bin/rm -f ./geologic $(OBJS)
	cd test ; make clean
