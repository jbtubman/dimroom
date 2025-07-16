cmake_minimum_required(VERSION 4.0)

project(test_dimroom)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# include(FetchContent)

# FetchContent_Declare(CppUnitTestFramework
#   GIT_REPOSITORY git@github.com:jbtubman/geologic.git
#   GIT_TAG master)

# FetchContent_MakeAvailable(CppUnitTestFramework)

# FetchContent_GetProperties(CppUnitTestFramework
#   SOURCE_DIR CppUnitTestFramework_SOURCE_DIR)



file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(test_dimroom
  # ${CMAKE_CURRENT_SOURCE_DIR}/src/test_dimroom.cpp
  ${MY_SOURCES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/command_handler.cpp
  ${PROJECT_SOURCE_DIR}/../src/command_interpreter.cpp
  ${PROJECT_SOURCE_DIR}/../src/query.cpp)

# target_link_libraries(test_dimroom PRIVATE CppUnitTestFramework)

# target_include_directories(CppUnitTestFramework
#   PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include" )

target_include_directories(
  test_dimroom
  PUBLIC "${CMAKE_SOURCE_DIR}/include"
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# add_library(CppUnitTestFramework)

# set_property(TARGET "test_dimroom" PROPERTY CXX_STANDARD 23)
