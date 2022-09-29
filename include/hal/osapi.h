//
// Created by tyll on 2022-09-28.
//

#ifndef SGLOGGER_OSAPI_H
#define SGLOGGER_OSAPI_H

#include <stddef.h>

namespace core {
    void findUsbSerialPortByProductId(char* devicePath, size_t n, const char* vid, const char* pid);

    template <size_t n>
    void findUsbSerialPortByProductId(char (&devicePath)[n], const char* vid, const char* pid) {
        findUsbSerialPortByProductId(devicePath, n, vid, pid);
    }
}

#endif //SGLOGGER_OSAPI_H
