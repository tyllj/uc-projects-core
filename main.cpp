
#ifdef CORE_TESTAPP

#include "hal/env.h"
#include "core/io/ConsoleWriter.h"
#include "core/io/ConsoleReader.h"
#include "core/platform/pc/SerialPort.h"
#include "core/io/StreamWriter.h"
#include "core/io/StreamReader.h"
#include "core/cli/LineEditor.h"
#include "core/cli/StringMap.h"
#include "core/cli/Echo.h"
#include "core/cli/Ls.h"
#include "core/cli/Cat.h"
#include "etl/delegate.h"
#include "core/io/FileSystemInfo.h"
#include "core/io/FileSystem.h"
#include "core/platform/pc/FileSystemImpl.h"
#include "core/cli/Ush.h"
#include "core/platform/pc/UsbCanSeeed.h"
#include "core/coop/Future.h"
#include "core/unique_ptr.h"
#include "core/coop/MainLoopDispatcher.h"
#include "core/can/IsoTpSocket.h"
#include "core/can/elm/ElmCommandInterpreter.h"
#include "core/can/obd/ObdRequest.h"
#include "core/can/obd/OnBoardDiagnostics.h"
#include "core/Tick.h"
#include "core/coop/DispatcherTimer.h"

int main() {
    core::initializeEnvironment();
    core::coop::MainLoopDispatcher<16> dispatcher;

    core::CString portName = core::platform::pc::usb::findCh340();
    if (core::cstrings::isNullOrEmpty(portName.get()))
        return 1;
    core::platform::pc::SerialPort usb(portName.get());
    usb.baudRate(core::can::USBCAN_SERIAL_BAUD);
    usb.open();

    core::can::UsbCanSeeed can(usb);
/*
    core::platform::pc::SerialPort tty("COM3");
    tty.baudRate(38400);
    tty.open();

    core::can::elm::ElmCommandInterpreter elm(dispatcher, tty, can);
*/
    core::io::ConsoleWriter out;
    core::can::obd::OnBoardDiagnostics obd(dispatcher, can);

    struct Closure {
        core::coop::IDispatcher& dispatcher;
        core::io::ConsoleWriter& out;
        core::can::obd::OnBoardDiagnostics& obd;
    };
    Closure c { dispatcher, out, obd };
    core::coop::DispatcherTimer<Closure> timer([](Closure c){
        core::can::obd::ObdRequest req;
        req.add(0);
        auto task = c.obd.getCurrentData(req).continueWith<Closure>(c, [](auto c, auto result){
            core::StringBuilder sb;
            auto pid = result->getByPid(0);
            sb.appendHex(pid.A);
            sb.appendHex(pid.B);
            sb.appendHex(pid.C);
            sb.appendHex(pid.D);
            c.out.writeLine(sb.toCString());
        });
        c.dispatcher.run(task);

        }, c, 5000);

    dispatcher.run(timer);

    for (;;) {
        dispatcher.dispatchOne();
        core::sleepms(2);
    }
}

#endif
