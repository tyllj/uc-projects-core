//
// Created by tyll on 2022-01-22.
//

#ifndef UC_CORE_PC_SERIALPORT_H
#define UC_CORE_PC_SERIALPORT_H

#if defined(__linux__)
#include "linux/LinuxSerialPort.h"

namespace core { namespace platform { namespace pc {
    typedef platform::pc::LinuxSerialPort SerialPort;
}}}

#elif defined(_WIN32)
#include "win32/Win32SerialPort.h"

namespace core { namespace platform { namespace pc {
    typedef platform::pc::Win32SerialPort SerialPort;
}}}
#endif

#endif //UC_CORE_SERIALPORT_H
