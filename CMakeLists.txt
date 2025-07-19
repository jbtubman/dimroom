cmake_minimum_required(VERSION 4.0)

project(dimroom VERSION 2.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(dimroom_VERSION_MAJOR 2)
set(dimroom_VERSION_MINOR 5)

configure_file(${PROJECT_SOURCE_DIR}/include/dimroomConfig.h.in ${PROJECT_SOURCE_DIR}/include/dimroomConfig.h)

add_executable(dimroom
  ${PROJECT_SOURCE_DIR}/src/dimroom.cpp
  ${PROJECT_SOURCE_DIR}/src/command_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/command_interpreter.cpp
  ${PROJECT_SOURCE_DIR}/src/query.cpp
)

target_include_directories(dimroom PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(test)
