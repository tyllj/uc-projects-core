
#ifdef CORE_TESTAPP
#define ETL_NO_STL

#include <unistd.h>     //STDIN_FILENO
#include "core/io/ConsoleWriter.h"
#include "core/io/ConsoleReader.h"
#include "core/io/ConcurrentQueueingReaderWriter.h"
#include "core/io/ports/SerialPort.h"
#include "core/io/StreamWriter.h"
#include "core/io/StreamReader.h"
#include "core/cli/LineEditor.h"
#include "core/cli/StringMap.h"
#include "core/cli/Echo.h"
#include "core/cli/Sgsh.h"
#include "core/threading/Sleep.h"
#include "core/can/ObdTransceiver.h"
#include "core/can/Channel.h"
#include "etl/delegate.h"

int main() {
    core::io::ports::SerialPort port;
    port.setPortName("COM3");
    port.setBaudRate(9600);
    port.open();

    core::io::StreamReader ttyin(port);
    core::io::StreamWriter ttyout(port);
    ttyout.setNewLine(core::cstrings::MSDOS);

    core::shared_ptr<core::cli::StringMap<core::cli::ProgramFactory, 128>> programs(new core::cli::StringMap<core::cli::ProgramFactory, 128>());
    programs->addOrUpdate("echo", []() { return core::shared_ptr<core::cli::CliProgram>(new core::cli::Echo()); });
    programs->addOrUpdate("sgsh", [&]() { return core::shared_ptr<core::cli::CliProgram>(new core::cli::Sgsh(*programs)); });

    core::io::DirectoryInfo wd;
    core::shared_ptr<core::cli::CliProgram> shell(new core::cli::Sgsh(*programs));

    shell->execute(ttyin, ttyout, wd, 0, nullptr);

    port.close();
    return 0;
}

#endif