cmake_minimum_required(VERSION 3.21)
project(SgLogger)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)
add_executable(SgLogger main.cpp include/io/StreamWriter.h include/io/Stream.h)