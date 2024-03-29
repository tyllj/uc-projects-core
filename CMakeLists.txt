cmake_minimum_required(VERSION 3.19)
project(uc-core C CXX)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-fexceptions -Wall -Wextra -Wno-pedantic -Wno-unused-parameter -Werror)

include_directories(include)
include_directories(lib)
include_directories(lib/etl/include)

set_source_files_properties(lib/iso15765-canbus/src/lib_iso15765.c PROPERTIES COMPILE_FLAGS "-Wno-pedantic -Wno-implicit-fallthrough -O3 -Wfatal-errors -Wall -Werror -std=c11")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
add_compile_definitions(CORE_TESTAPP)
add_compile_definitions(ETL_NO_STL ETL_FORCE_STD_INITIALIZER_LIST ETL_THROW_EXCEPTIONS)
add_executable(testapp testapp.cpp)
target_sources(testapp PUBLIC src/platform/pc/env.cpp)
target_sources(testapp PUBLIC lib/iso15765-canbus/src/lib_iso15765.c lib/iso15765-canbus/src/lib_iqueue.c)

IF(WIN32)
    target_sources(testapp PUBLIC src/platform/pc/win32/tick.cpp)
    target_link_libraries(testapp setupapi.lib)
ELSEIF(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_sources(testapp PUBLIC src/platform/pc/linux/tick.cpp)
    find_package(Curses QUIET)
    IF(Curses_FOUND)
        include_directories(${CURSES_INCLUDE_DIR})
    ENDIF()
    IF(Curses_FOUND)
        target_link_libraries(testapp ${CURSES_LIBRARIES})
    ENDIF()
ENDIF()

