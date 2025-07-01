# Copyright 2025, James B. Tubman
CPP = /usr/bin/c++

geologic: main.cpp
	$(CPP) -std=c++23 -o geologic main.cpp
