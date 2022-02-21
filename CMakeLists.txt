cmake_minimum_required(VERSION 3.19)
project(SgLogger)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)
include_directories(lib)
include_directories(lib/etl/include)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(main main.cpp lib/RS-232/rs232.c include/core/Math.h include/core/Bits.h)
target_link_libraries(main ${CURSES_LIBRARIES})