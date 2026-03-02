cmake_minimum_required(VERSION 4.2)

include(GoogleTest)

add_executable(test_dimroom
  ${CMAKE_CURRENT_SOURCE_DIR}/src/test_dimroom.cpp
  ${PROJECT_SOURCE_DIR}/src/command_line.cpp
  ${PROJECT_SOURCE_DIR}/src/query.cpp)

if(READLINE_FOUND)
  target_include_directories(
    test_dimroom
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    ${Readline_INCLUDE_DIR})
  target_link_libraries(test_dimroom ${Readline_LIBRARY} gtest)
else()
  target_include_directories(
    test_dimroom
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
  target_link_libraries(test_dimroom gtest)
endif()

gtest_discover_tests(test_dimroom)
