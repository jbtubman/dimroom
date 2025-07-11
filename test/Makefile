CPP = /usr/bin/c++

CFLAGS = -std=c++23 -I../../CppUnitTestFramework -I..

UPPER_DIR = ..

.SUFFIXES : .o .cpp .hpp

OBJS = main.o cell_test.o cell_types_test.o column_test.o coordinates_test.o parse_utils_test.o parser_test.o row_test.o table_test.o

SRCS := $(patsubst %.o,%.cpp,$(OBJS))

UPPER_INCLUDES = $(wildcard ../*.hpp)

TEST_INCS := $(patsubst %_test.cpp,../%.hpp,$(wildcard *_test.cpp))

test: $(OBJS) ../parser.o $(TEST_INCS)
	printf "TEST_INCS: \"%s\"" "$(TEST_INCS)"
	$(CPP) $(CFLAGS) -o $@ $(OBJS) ../parser.o

*_test.cpp : $(TEST_INCS)
#	$(CPP) $(CFLAGS) -c $<

# $(OBJS) : $(SRCS)

.cpp.o :
	$(CPP) $(CFLAGS) -c $<

.PHONY: ../parser.o
../parser.o : ../parser.cpp ../parser.hpp ../cell_types.hpp ../column.hpp ../jt_concepts.hpp ../parse_utils.hpp ../utility.hpp
	cd .. ; make parser.o

.PHONY : clean
clean:
	/bin/rm -f test $(OBJS)
