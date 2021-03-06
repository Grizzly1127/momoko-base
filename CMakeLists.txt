cmake_minimum_required(VERSION 2.6)
project(momoko C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CXX_FLAGS
    -g
    -O0
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wno-unused-parameter
    -Wold-style-cast
    -Woverloaded-virtual
    -Wpointer-arith
    -Wshadow
    -Wwrite-strings
    -march=native
    -std=c++11
    -rdynamic
    )

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(momoko-base)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(example)
endif()
