
#ifdef CORE_TESTAPP
#define ETL_NO_STL

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
#include "core/cli/Sgsh.h"
#include "core/Sleep.h"
#include "core/can/ObdTransceiver.h"
#include "core/can/Channel.h"
#include "etl/delegate.h"
#include "core/io/FileSystemInfo.h"
#include "core/io/FileSystem.h"
#include "core/platform/pc/FileSystemImpl.h"

int main() {
    core::io::NativeFileSystem fs;
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
    programs->addOrUpdate("sgsh", [&]() { return core::shared_ptr<core::cli::CliProgram>(new core::cli::Sgsh(*programs)); });

    core::io::DirectoryInfo wd(fs, "/Test");
    core::shared_ptr<core::cli::CliProgram> shell(new core::cli::Sgsh(*programs));

    shell->execute(ttyin, ttyout, wd, 0, nullptr);

    uart.close();
    return 0;
}

#endif