
#ifdef CORE_TESTAPP
#include <stdexcept>
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
#include "core/Future.h"
#include "core/unique_ptr.h"
#include "core/coop/MainLoopDispatcher.h"
#include "core/can/IsoTpSocket.h"
#include "core/can/elm/ElmCommandInterpreter.h"
#include "core/can/obd/ObdRequest.h"
#include "core/can/obd/OnBoardDiagnostics.h"
#include "core/Tick.h"
#include "core/coop/DispatcherTimer.h"
#include "core/platform/pc/FileSystemImpl.h"
#include "core/can/obd/WellKnownPids.h"
#include "core/Try.h"

int main() {
    core::initializeEnvironment();
    core::coop::MainLoopDispatcher<16> dispatcher;

    core::CString portName = DARE(core::platform::pc::usb::findCh340());

    auto usb = DARE(core::platform::pc::SerialPort::open(portName, core::can::USBCAN_SERIAL_BAUD));
    core::can::UsbCanSeeed can(usb);

    core::io::ConsoleWriter out;
    core::io::ConsoleReader in;
    core::can::obd::OnBoardDiagnostics obd(dispatcher, can);

    core::can::obd::ObdRequest query;
    DARE(query.add(core::can::obd::wellKnownPids[0x0C].pid));

    while (in.read() == -1) {
        auto rpm = DARE(obd.getCurrentData(query)
                .awaitOn(dispatcher)
                .at(0));
        out.writeLine(core::StringBuilder() + "EngineSpeed=" + core::can::obd::toString(rpm));
    }
}

#endif
