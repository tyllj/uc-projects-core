//
// Created by tyll on 11.09.22.
//

#ifndef UC_CORE_USB_H
#define UC_CORE_USB_H

#if defined(__linux__)
#include "linux/LinuxUsb.h"
#elif defined(_WIN32)
#include "win32/Win32Usb.h"
#endif

#include "core/CStrings.h"
#include <string.h>
#include <stdio.h>


namespace core { namespace platform { namespace pc { namespace usb {
    constexpr char CH340_PRODUCT_ID[] = "1a86:7523";
    constexpr char FTDI_PRODUCT_ID0[] = "0403:6001";
    constexpr char FTDI_PRODUCT_ID1[] = "0403:6011";
    constexpr char ARDUINO_PRODUCT_ID[] = "03eb:204b";

    template<size_t n>
    void findUsbSerialPortByProductId(char (&devicePath)[n], const char* vid, const char* pid) {
        findUsbSerialPortByProductId(devicePath, n, vid, pid);
    }

    core::CString findPortNameByUsbProductId(const char* vid, const char* pid) {
        char devicePath[32] = { 0 };
        findUsbSerialPortByProductId(devicePath, vid, pid);
        return core::cstrings::toSharedCString(devicePath);
    }

    core::CString findPortNameByUsbProductId(const char* pidColonVid) {
        char vid[5] = {0};
        char pid[5] = {0};

        if (core::cstrings::length(pidColonVid) != 9)
            return {};
        core::cstrings::copyTo(vid, pidColonVid, 4);
        core::cstrings::copyTo(pid, &pidColonVid[5], 4);
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
#endif //UC_CORE_OSAPI_H
