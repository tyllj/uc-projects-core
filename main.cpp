#define ETL_NO_STL

#include <iostream>
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
#include "core/threading/sleep.h"

struct locals_t {
    core::io::ConsoleReader& in;
    core::io::ConsoleWriter& out;
    core::io::StreamReader<core::io::ports::SerialPort>& ttyin;
    core::io::StreamWriter<core::io::ports::SerialPort>& ttyout;
    core::io::ConcurrentQueueingReaderWriter<128>& inout;
};

void* listenOnConsole(void* tl) {
    locals_t& locals = *(locals_t*) tl;

    for(;;) {
        int32_t c = locals.ttyin.read();
        if (c != -1)
            locals.inout.write(c);
        sleepms(50);
    }
    return nullptr;
}
int main() {
    core::io::ConsoleReader in;
    core::io::ConsoleWriter out;
    core::io::ports::SerialPort port;
    port.setPortName("COM3");
    port.setBaudRate(9600);
    core::io::StreamReader ttyin(port);
    core::io::StreamWriter ttyout(port);
    ttyout.setNewLine(core::cstrings::MSDOS);
    core::io::ConcurrentQueueingReaderWriter<128> inout;
    core::cli::LineEditor<128> commandLine(inout, ttyout);
    locals_t locals {in, out, ttyin, ttyout, inout};

    core::shared_ptr<core::cli::StringMap<core::cli::ProgramFactory, 128>> programs(new core::cli::StringMap<core::cli::ProgramFactory, 128>());
    programs->addOrUpdate("echo", []() { return core::shared_ptr<core::cli::CliProgram>(new core::cli::Echo()); });
    programs->addOrUpdate("sgsh", [&]() { return core::shared_ptr<core::cli::CliProgram>(new core::cli::Sgsh(*programs)); });

    port.open();

    //pthread_t listenerThread;
    //pthread_create(&listenerThread, NULL, listenOnConsole, &locals);

    core::io::Directory wd;
    core::shared_ptr<core::cli::CliProgram> shell(new core::cli::Sgsh(*programs));

    shell->execute(ttyin, ttyout, wd, 0, nullptr);

    //pthread_cancel(listenerThread);
    return 0;
}
