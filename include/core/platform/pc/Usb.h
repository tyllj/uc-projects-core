//
// Created by tyll on 11.09.22.
//

#ifndef SGLOGGER_USB_H
#define SGLOGGER_USB_H

#include "core/CStrings.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>


namespace core { namespace io { namespace ports { namespace usb {
    constexpr char CH340_PRODUCT_ID[] = "1a86:7523";
    constexpr char FTDI_PRODUCT_ID0[] = "0403:6001";
    constexpr char FTDI_PRODUCT_ID1[] = "0403:6011";
    constexpr char ARDUINO_PRODUCT_ID[] = "03EB:0x204B";

    static constexpr char LINUX_QUERY_VID[] = "udevadm info -a -n %s | grep '{idVendor}' | head -n1";
    static constexpr char LINUX_QUERY_PID[] = "udevadm info -a -n %s | grep '{idProduct}' | head -n1";

    static core::CString _runCommand(const char* c) {
        FILE* fp;
        char buf[128];
        core::shared_ptr<char[]> result;
        /* Open the command for reading. */
        fp = popen(c, "r");
        if (fp == NULL) {
            printf("Failed to run command\n" );
            exit(1);
        }

        /* Read the output a line at a time - output it. */
        if (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
            result = core::cstrings::toSharedCString(buf);
        }

        /* close */
        pclose(fp);
        return result;
    }

    core::CString findPortNameByUsbProductId(const char* vid, const char* pid) {
        char devicePath[32];
        char command[128];
        for (uint8_t i = 0; i < 32; i++) {
            snprintf(devicePath, sizeof(devicePath) - 1, "/dev/ttyUSB%i", i);
            if (access(devicePath, F_OK) != 0)
                continue; // device path doesn't exist, try next

            snprintf(command, sizeof(command) - 1, LINUX_QUERY_VID, devicePath);
            shared_ptr<char[]> result = _runCommand(command);
            if (core::cstrings::isNullOrEmpty(result.get())
                || strchr(result.get(), '\"') == nullptr)
                return {};

            char pidOrVid[5] = {'\0'};
            strncpy(pidOrVid, strchr(result.get(), '\"') + 1, 4);
            if (!core::cstrings::equalsCaseInvariant(pidOrVid, vid))
                continue;

            snprintf(command, sizeof(command) - 1, LINUX_QUERY_PID, devicePath);
            result = _runCommand(command);
            if (core::cstrings::isNullOrEmpty(result.get())
                || strchr(result.get(), '\"') == nullptr)
                return {};

            strncpy(pidOrVid, strchr(result.get(), '\"') + 1, 4);
            if (!core::cstrings::equalsCaseInvariant(pidOrVid, pid))
                continue;

            return core::cstrings::toSharedCString(devicePath);
        }
        return {};
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
#endif //SGLOGGER_USB_H
