cmake_minimum_required(VERSION 4.0)

project(test_dimroom)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(test_dimroom
  ${TEST_SOURCES}
  ${PROJECT_SOURCE_DIR}/../src/command_handler.cpp
  ${PROJECT_SOURCE_DIR}/../src/command_interpreter.cpp
  ${PROJECT_SOURCE_DIR}/../src/query.cpp)

target_include_directories(
  test_dimroom
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include"
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
