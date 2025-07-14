cmake_minimum_required(VERSION 4.0)

project(test_dimroom)
set(CMAKE_CXX_STANDARD 23)

add_executable(test_dimroom main.cpp)

find_package(CppUnitTestFramework REQUIRED COMPONENTS CXX)
