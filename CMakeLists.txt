cmake_minimum_required(VERSION 3.21)
project(SgLogger)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)
include_directories(lib)
add_executable(SgLogger main.cpp lib/RS-232/rs232.c include/core/io/StreamWriter.h include/core/io/Stream.h include/core/io/QueueStream.h include/core/io/ConcurrentQueueingReaderWriter.h include/core/threading/INativeMutex.h include/core/threading/LockGuard.h include/core/threading/PosixMutex.h include/core/io/ports/SerialPort.h include/core/io/StreamReader.h include/core/cli/LineEditor.h include/core/cli/AnsiCharacters.h include/core/cli/CliProgram.h include/core/io/File.h include/core/io/Directory.h include/core/cli/StringMap.h include/core/shared_ptr.h include/core/cli/CommandNotFound.h include/core/cli/Echo.h include/core/cli/Sgsh.h include/core/CStrings.h include/core/collections/TemplatedMap.h include/core/collections/TemplatedStringMap.h)
