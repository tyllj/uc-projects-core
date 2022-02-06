cmake_minimum_required(VERSION 3.21)
project(SgLogger)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)
include_directories(lib)
include_directories(lib/etl/include)
add_executable(SgLogger main.cpp lib/RS-232/rs232.c)
