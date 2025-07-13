CPP = /usr/bin/c++

CFLAGS = -std=c++23 -I../../CppUnitTestFramework -I..

UPPER_DIR = ..

.SUFFIXES : .o .cpp .hpp

OBJS = main.o cell_test.o cell_types_test.o column_test.o command_interpreter_test.o coordinates_test.o \
	parse_utils_test.o parser_test.o row_test.o table_test.o utility_test.o query_test.o

SRCS := $(patsubst %.o,%.cpp,$(OBJS))

UPPER_INCLUDES = $(wildcard ../*.hpp)

TEST_INCS := $(patsubst %_test.cpp,../%.hpp,$(wildcard *_test.cpp))

test: $(OBJS) ../parser.o ../query.o ../command_interpreter.o $(TEST_INCS)
	printf "TEST_INCS: \"%s\"" "$(TEST_INCS)"
	$(CPP) $(CFLAGS) -o $@ $(OBJS) ../parser.o ../query.o ../command_interpreter.o

*_test.cpp : $(TEST_INCS)

main.cpp:

main.o: main.cpp
	$(CPP) $(CFLAGS) -c $<

query_test.cpp: ../query.hpp ../query.hpp

query_test.o: query_test.cpp
	$(CPP) $(CFLAGS) -c $<

table_test.cpp: ../table.hpp ../parser.hpp

table_test.o: table_test.cpp
	$(CPP) $(CFLAGS) -c $<

cell_test.cpp: ../cell.hpp ../parser.hpp

cell_test.o: cell_test.cpp ../cell.hpp ../parser.hpp
	$(CPP) $(CFLAGS) -c $<

parser_test.cpp: ../parser.hpp ../parser.cpp

parser_test.o: parser_test.cpp
	$(CPP) $(CFLAGS) -c $<

.cpp.o :
	$(CPP) $(CFLAGS) -c $<

.PHONY: ../parser.o
../parser.o : ../parser.cpp ../parser.hpp ../cell_types.hpp ../column.hpp ../jt_concepts.hpp ../parse_utils.hpp ../utility.hpp
	cd .. ; make parser.o ; cd test

.PHONY: ../query.o
../query.o : ../query.cpp ../query.hpp ../table.hpp
	cd .. ; make query.o ; cd test

.PHONY: ../command_interpreter.o
../command_interpreter.o : ../command_interpreter.cpp ../command_interpreter.hpp
	cd .. ; make command_interpreter.o ; cd test

.PHONY : clean
clean:
	/bin/rm -f test $(OBJS)
