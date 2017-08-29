cmake_minimum_required(VERSION 3.7)
project(worker)

include(cmake/findDependencies.cmake      REQUIRED)
include(cmake/compilerOptions.cmake       REQUIRED)

add_subdirectory(worker)

set(SOURCE_FILES main.cpp)
add_executable(program ${SOURCE_FILES})

target_link_libraries(program PRIVATE worker)
