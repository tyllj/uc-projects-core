
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
#include "core/platform/pc/UsbCan.h"
#include "core/async/Task.h"
int main() {
/*
    uint8_t x = 0;
    core::async::Task<uint8_t*, void*> testTask(&x, [](core::async::TaskContext<uint8_t*, void*>& context) {
        uint8_t& x_local = *(context.getData());
        printf("Counter is %i in testTask.\n", x_local);
        x_local += 1;
        if (x_local >= 8)
            context.setResult(&x_local);
    });

    testTask.continueWith([](etl::optional<void*> result) {
        printf("Result is %i.", *reinterpret_cast<uint8_t*>(result.value()));
    }).wait();
*/

    core::io::NativeFileSystem fs;
    core::io::ports::SerialPort tty("COM3");
    core::io::ports::SerialPort usb("COM8");
    tty.setBaudRate(9600);
    tty.open();

    usb.setBaudRate(115200);
    usb.open();

    core::can::UsbCan can(usb);

    core::io::StreamReader ttyin(tty);
    core::io::StreamWriter ttyout(tty);
    ttyout.setNewLine(core::cstrings::MSDOS);

    core::shared_ptr<core::cli::StringMap<core::cli::ProgramFactory, 128>> programs(new core::cli::StringMap<core::cli::ProgramFactory, 128>());
    programs->addOrUpdate("echo", []() { return core::shared_ptr<core::cli::CliProgram>(new core::cli::Echo()); });
    programs->addOrUpdate("ls", []() { return core::shared_ptr<core::cli::CliProgram>(new core::cli::Ls()); });
    programs->addOrUpdate("cat", []() { return core::shared_ptr<core::cli::CliProgram>(new core::cli::Cat()); });

    core::io::DirectoryInfo wd(fs, "/Test");
    core::cli::Ush shell(*programs, ttyin, ttyout, wd);
    while (!shell.isExit()) {
        if (!shell.accept())
            sleepms(20);

        can.receive();
    }

    usb.close();
    tty.close();

    return 0;
}

#endif