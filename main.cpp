
#ifdef CORE_TESTAPP

#include <unistd.h>     //STDIN_FILENO
#include "core/io/ConsoleWriter.h"
//#include "core/io/ConsoleReader.h"
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
#include "etl/delegate.h"
#include "core/io/FileSystemInfo.h"
#include "core/io/FileSystem.h"
#include "core/platform/pc/FileSystemImpl.h"
#include "core/cli/Ush.h"
#include "core/platform/pc/UsbCanSeeed.h"
#include "core/async/Future.h"
#include "core/unique_ptr.h"
#include "core/async/MainLoopDispatcher.h"
#include "core/can/IsoTpSocket.h"
//#include "core/can/elm/ElmCanInterface.h"
#include "core/can/elm/ElmCommandInterpreter.h"

int main() {
    core::async::MainLoopDispatcher<16> dispatcher;

    for (;;) {
        dispatcher.dispatchOne();

    }
}

#endif
