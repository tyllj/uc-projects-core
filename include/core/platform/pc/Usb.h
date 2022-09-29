//
// Created by tyll on 11.09.22.
//

#ifndef SGLOGGER_OSAPI_H
#define SGLOGGER_USB_H

#include "core/CStrings.h"
#include "hal/osapi.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>


namespace core { namespace io { namespace ports { namespace usb {
    constexpr char CH340_PRODUCT_ID[] = "1a86:7523";
    constexpr char FTDI_PRODUCT_ID0[] = "0403:6001";
    constexpr char FTDI_PRODUCT_ID1[] = "0403:6011";
    constexpr char ARDUINO_PRODUCT_ID[] = "03eb:204b";


    core::CString findPortNameByUsbProductId(const char* vid, const char* pid) {
        char devicePath[32] = { 0 };
        core::findUsbSerialPortByProductId(devicePath, vid, pid);
        return core::cstrings::toSharedCString(devicePath);
    }

    core::CString findPortNameByUsbProductId(const char* pidColonVid) {
        char vid[5];
        char pid[5];

        if (core::cstrings::length(pidColonVid) != 9)
            return {};
        strncpy(vid, pidColonVid, 4);
        strncpy(pid, &pidColonVid[5], 4);
        return findPortNameByUsbProductId(vid, pid);
    }

    core::CString findCh340() {
        return findPortNameByUsbProductId(CH340_PRODUCT_ID);
    }

    core::CString findFtdi() {
        core::CString result = findPortNameByUsbProductId(FTDI_PRODUCT_ID0);
        if (!result)
            result = findPortNameByUsbProductId(FTDI_PRODUCT_ID1);
        return result;
    }

    core::CString findArduino() {
        return findPortNameByUsbProductId(ARDUINO_PRODUCT_ID);
    }
}}}}
#endif //SGLOGGER_OSAPI_H
