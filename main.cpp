
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
#include "core/async/Future.h"
#include "core/unique_ptr.h"
#include "core/async/MainLoopDispatcher.h"

core::unique_ptr<char[]> getY() {
    return core::unique_ptr<char[]>(new char[] {"Hallo Welt"});
}

int main() {
    core::async::MainLoopDispatcher<8> dispatcher;

    core::io::ports::SerialPort usb("COM9");
    usb.setBaudRate(115200);
    usb.open();
    core::can::UsbCan can(dispatcher, usb);
    core::io::ConsoleReader reader;

    etl::delegate<void(core::can::CanFrame)> onReceived([] (core::can::CanFrame frame) {
        for (uint8_t i; i<frame.length; i++)
            putch(frame.payload[i]);
        putch('\n');
    });
    can.newData() += onReceived;
    bool quit(false);
    while(!quit) {
        dispatcher.dispatchOne();
        sleepms(1);

        int32_t r;
        if (-1 != (r=reader.read())) {
            if (uint8_t(r) == 'q')
                quit = true;

            core::can::CanFrame f;
            f.length=1;
            f.id=100;
            for (uint8_t i = 0; i < 8; i++) f.payload[i] = 0;
            printf("Sending char: %c\n", uint8_t(r));
            f.payload[0] = uint8_t(r);
            can.send(f);
        }
    }

/*
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
    */
}

#endif