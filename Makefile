# Copyright 2025, James B. Tubman
CPP = /usr/bin/c++

CFLAGS = -std=c++23

# TEST_INCLUDE_DIR = -I../CppUnitTestFramework

.SUFFIXES : .o .cpp

OBJS = main.o command_interpreter.o query.o

# TEST_OBJS = test/*.o

.cpp.o :
	$(CPP) $(CFLAGS) -c $<

dimroom: $(OBJS)
	$(CPP) -std=c++23 -o $@ $(OBJS)

.PHONY: test/test
test/test: test/main.cpp parser.hpp cell_types.hpp \
	column.hpp jt_concepts.hpp parse_utils.hpp utility.hpp query.o command_handler.o
	cd test ; make test
#	$(CPP) -std=c++23 -I../CppUnitTestFramework $(?) -o test/test

.PHONY: do_test
do_test: test/test
	./test/test

cell.hpp: cell_types.hpp column.hpp coordinates.hpp parser.hpp

cell_types.hpp: coordinates.hpp utility.hpp

column.hpp: cell_types.hpp

command_handler.o: command_handler.cpp

command_handler.cpp: command_handler.hpp

command_handler.hpp: coordinates.hpp query.hpp table.hpp

command_interpreter.o: command_handler.cpp

command_interpreter.cpp: command_interpreter.hpp

command_interpreter.hpp: command_handler.hpp query.hpp table.hpp

# no in-project dependencies
contains.hpp:

coordinates_formatter.hpp: coordinates.hpp

# no in-project dependencies
coordinates.hpp:

parse_utils.hpp: cell_types.hpp coordinates.hpp jt_concepts.hpp utility.hpp

parser.hpp: cell_types.hpp column.hpp jt_concepts.hpp parse_utils.hpp utility.hpp

query.hpp: table.hpp

query.cpp: query.hpp

query.o: query.cpp

row.hpp: cell.hpp cell_types.hpp parse_utils.hpp parser.hpp

table.hpp: column.hpp cell.hpp row.hpp parser.hpp

utility.hpp: jt_concepts.hpp

main.cpp: cell_types.hpp command_interpreter.hpp contains.hpp coordinates_formatter.hpp coordinates.hpp table.hpp

clean:
	/bin/rm -f ./geologic $(OBJS)
	cd test ; make clean
