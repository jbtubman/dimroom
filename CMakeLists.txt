cmake_minimum_required(VERSION 4.0)

project(dimroom VERSION 2.17)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Readline)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0 # Or a specific tag/commit
  FIND_PACKAGE_ARGS NAMES GTest
)
FetchContent_MakeAvailable(googletest)

find_package(googletest)

set(dimroom_VERSION_MAJOR 2)
set(dimroom_VERSION_MINOR 17)

configure_file(${PROJECT_SOURCE_DIR}/include/dimroomConfig.h.in ${PROJECT_SOURCE_DIR}/include/dimroomConfig.h)

add_executable(dimroom
  ${PROJECT_SOURCE_DIR}/src/dimroom.cpp
  ${PROJECT_SOURCE_DIR}/src/command_line.cpp
  ${PROJECT_SOURCE_DIR}/src/query.cpp
)

if(READLINE_FOUND)
  target_include_directories(dimroom PUBLIC ${PROJECT_SOURCE_DIR}/include
    ${Readline_INCLUDE_DIR})
  target_link_libraries(dimroom ${Readline_LIBRARY} gtest)
else()
  target_include_directories(dimroom PUBLIC ${PROJECT_SOURCE_DIR}/include)
endif()

add_subdirectory(test)
