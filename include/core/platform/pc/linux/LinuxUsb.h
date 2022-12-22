//
// Created by tyll on 29.09.22.
//

#ifndef UC_CORE_LINUXUSB_H
#define UC_CORE_LINUXUSB_H
#if defined (__linux__)

#include <stdexcept>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "core/CStrings.h"

namespace core { namespace platform { namespace pc { namespace usb {
    static constexpr char LINUX_QUERY_VID[] = "udevadm info -a -n %s | grep '{idVendor}' | head -n1";
    static constexpr char LINUX_QUERY_PID[] = "udevadm info -a -n %s | grep '{idProduct}' | head -n1";

    auto findUsbSerialPortByProductId(char *devicePath, size_t n, const char *vid, const char *pid) -> core::ErrorOr<void> {
        auto runCommand = [](char *result, size_t n, const char *c) -> core::ErrorOr<void> {
            FILE *fp;
            fp = popen(c, "r");
            VERIFY(fp != nullptr, "Could not run command.");
            auto closeProcess = core::Defer([fp] { pclose(fp); });

            /* Read first line of output. */
            VERIFY(fgets(result, n - 1, fp) != NULL, "Command did not generate any output.")
            return {}
        };

        char command[128];
        char cmdOut[128];
        for (uint8_t i = 0; i < 32; i++) {
            snprintf(devicePath, n - 1, "/dev/ttyUSB%i", i);
            if (access(devicePath, F_OK) != 0)
                continue; // device path doesn't exist, try next

            snprintf(command, sizeof(command), LINUX_QUERY_VID, devicePath);
            runCommand(cmdOut, sizeof(cmdOut), command);
            if (core::cstrings::isNullOrEmpty(cmdOut)
                || !core::cstrings::contains(cmdOut, '\"')) {
                core::cstrings::empty(devicePath);
                return core::AssertionError("Unexpected command output.");
            }

            char pidOrVid[5] = {'\0'};
            strncpy(pidOrVid, strchr(cmdOut, '\"') + 1, 4);
            if (!core::cstrings::equalsCaseInvariant(pidOrVid, vid))
                continue;

            snprintf(command, sizeof(command), LINUX_QUERY_PID, devicePath);
            runCommand(cmdOut, sizeof(cmdOut), command);
            if (core::cstrings::isNullOrEmpty(cmdOut)
                || !core::cstrings::contains(cmdOut, '\"')) {
                core::cstrings::empty(devicePath);
                return core::AssertionError("Unexpected command output.");
            }

            strncpy(pidOrVid, strchr(cmdOut, '\"') + 1, 4);
            if (!core::cstrings::equalsCaseInvariant(pidOrVid, pid))
                continue;

            return {}; // device with current devicePath matches VID and PID.
        }
        core::cstrings::empty(devicePath);
        return core::Error(0x005BFA11, "No serial port found for this usb device type.");
    }
}}}}

#endif
#endif //SGLOGGER_LINUXUSB_H
