# Copyright 2025, James B. Tubman
CPP = /usr/bin/c++

geologic: main.cpp
	$(CPP) -std=c++23 main.cpp -o geologic

main.cpp: coordinates.hpp coordinate_formatter.hpp column.hpp

test/test: test/*.cpp
	$(CPP) -std=c++23 -I../CppUnitTestFramework $(?) -o test/test

do_test: test/test test/*.cpp
	./test/test

clean:
	/bin/rm -f ./geologic ./test/test
