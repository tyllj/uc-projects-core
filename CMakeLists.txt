cmake_minimum_required(VERSION 3.19)
project(SgLogger)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter -Werror)

include_directories(include)
include_directories(lib)
include_directories(lib/etl/include)

# For linux:
#find_package(Curses REQUIRED)
#include_directories(${CURSES_INCLUDE_DIR})

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
add_compile_definitions(CORE_TESTAPP ETL_NO_STL)
add_executable(main main.cpp lib/RS-232/rs232.c)
target_link_libraries(main ${CURSES_LIBRARIES})