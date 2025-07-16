cmake_minimum_required(VERSION 4.0)

project(dimroom VERSION 2.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(${PROJECT_SOURCE_DIR}/include/dimroomConfig.h.in ${PROJECT_SOURCE_DIR}/include/dimroomConfig.h)

add_executable(dimroom ${PROJECT_SOURCE_DIR}/src/dimroom.cpp ${PROJECT_SOURCE_DIR}/src/command_handler.cpp ${PROJECT_SOURCE_DIR}/src/command_interpreter.cpp src/query.cpp)

target_include_directories(dimroom PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# include(FetchContent)

# FetchContent_Declare(CppUnitTestFramework
# GIT_REPOSITORY git@github.com:jbtubman/geologic.git)

# FetchContent_MakeAvailable(CppUnitTestFramework)

add_subdirectory(test)

# enable_testing()

# add_subdirectory(test)

# set(CMAKE_CXX_STANDARD 23)

# set(TEST_PROJECT_NAME test_dimroom)

# set(TEST_CURRENT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")

# file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# # add_executable("${CMAKE_PROJECT_NAME}" "${CMAKE_CURRENT_SOURCE_DIR}/src/dimroom.cpp")
# add_executable(dimroom "${CMAKE_CURRENT_SOURCE_DIR}/src/dimroom.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/command_handler.cpp"
# "${CMAKE_CURRENT_SOURCE_DIR}/src/command_interpreter.cpp"
# "${CMAKE_CURRENT_SOURCE_DIR}/src/query.cpp")

# install(TARGETS dimroom DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}")

# target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES})

# target_include_directories(dimroom PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 23)

# add_subdirectory("test")

# add_executable(test_dimroom "${CMAKE_CURRENT_SOURCE_DIR}/test/src/test_dimroom.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/command_handler.cpp"
# "${CMAKE_CURRENT_SOURCE_DIR}/src/command_interpreter.cpp"
# "${CMAKE_CURRENT_SOURCE_DIR}/src/query.cpp")

# # file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${TEST_CURRENT_SOURCE_DIR}/src/*.cpp")
# target_include_directories("${TEST_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/test/include" PUBLIC PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# # target_sources("${TEST_PROJECT_NAME}" PRIVATE ${TEST_SOURCES})
# target_sources("${TEST_PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/test/src/test_dimroom.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/command_handler.cpp"
# "${CMAKE_CURRENT_SOURCE_DIR}/src/command_interpreter.cpp"
# "${CMAKE_CURRENT_SOURCE_DIR}/src/query.cpp")
