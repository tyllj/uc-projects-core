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
    constexpr char CP210X_PRODUCT_ID[] = "10c4:ea60";

    template<size_t n>
    auto findUsbSerialPortByProductId(char (&devicePath)[n], const char* vid, const char* pid) -> core::ErrorOr<void> {
        return findUsbSerialPortByProductId(devicePath, n, vid, pid);
    }

     auto findPortNameByUsbProductId(const char* vid, const char* pid) -> core::ErrorOr<core::CString> {
        char devicePath[32] = { 0 };
        TRY(findUsbSerialPortByProductId(devicePath, vid, pid));
        return core::CString(devicePath);
    }

    auto findPortNameByUsbProductId(const char* pidColonVid) -> core::ErrorOr<core::CString> {
        char vid[5] = {0};
        char pid[5] = {0};

        VERIFY(core::cstrings::length(pidColonVid) == 9, "USB product id has unexpected length.");
        core::cstrings::copyTo(vid, pidColonVid, 4);
        core::cstrings::copyTo(pid, &pidColonVid[5], 4);
        return findPortNameByUsbProductId(vid, pid);
    }

    auto findCh340() -> core::ErrorOr<core::CString> {
        return findPortNameByUsbProductId(CH340_PRODUCT_ID);
    }

    auto findFtdi() -> core::ErrorOr<core::CString> {
        auto result = findPortNameByUsbProductId(FTDI_PRODUCT_ID0);
        if (result.is_error())
            return findPortNameByUsbProductId(FTDI_PRODUCT_ID1);
        return result;
    }

    auto findArduino() -> core::ErrorOr<core::CString> {
        return findPortNameByUsbProductId(ARDUINO_PRODUCT_ID);
    }

    auto findCp210x() -> core::ErrorOr<core::CString> {
        return findPortNameByUsbProductId(CP210X_PRODUCT_ID);
    }
}}}}
#endif //UC_CORE_OSAPI_H
