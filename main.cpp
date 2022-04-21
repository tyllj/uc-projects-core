
#ifdef CORE_TESTAPP

#include <unistd.h>     //STDIN_FILENO
#include "core/io/ConsoleWriter.h"
#include "core/io/ConsoleReader.h"
#include "core/io/ConcurrentQueueingReaderWriter.h"
#include "core/platform/pc/SerialPort.h"
#include "core/io/StreamWriter.h"
#include "core/io/StreamReader.h"
#include "core/cli/LineEditor.h"
#include "core/cli/StringMap.h"
#include "core/cli/Echo.h"
#include "core/cli/Ls.h"
#include "core/cli/Cat.h"
#include "core/Sleep.h"
#include "core/can/ObdTransceiver.h"
#include "core/can/Channel.h"
#include "etl/delegate.h"
#include "core/io/FileSystemInfo.h"
#include "core/io/FileSystem.h"
#include "core/platform/pc/FileSystemImpl.h"
#include "core/cli/Ush.h"

int main() {
    //core::io::NativeFileSystem fs;
    core::io::ports::SerialPort uart;
    uart.setPortName("COM3");
    uart.setBaudRate(9600);
    uart.open();

    core::io::StreamReader ttyin(uart);
    core::io::StreamWriter ttyout(uart);
    ttyout.setNewLine(core::cstrings::MSDOS);

    core::shared_ptr<core::cli::StringMap<core::cli::ProgramFactory, 128>> programs(new core::cli::StringMap<core::cli::ProgramFactory, 128>());
    programs->addOrUpdate("echo", []() { return core::shared_ptr<core::cli::CliProgram>(new core::cli::Echo()); });
    programs->addOrUpdate("ls", []() { return core::shared_ptr<core::cli::CliProgram>(new core::cli::Ls()); });
    programs->addOrUpdate("cat", []() { return core::shared_ptr<core::cli::CliProgram>(new core::cli::Cat()); });

    core::io::DirectoryInfo wd(core::io::NullFileSystem, "/Test");
    core::cli::Ush shell(*programs, ttyin, ttyout, wd);
    while (!shell.isExit()) {
        if (!shell.accept())
            sleepms(20);
    }
//
    uart.close();
    return 0;
}

#endif