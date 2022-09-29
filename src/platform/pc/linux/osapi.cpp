//
// Created by tyll on 2022-09-28.
//
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))


#include <stdexcept>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "hal/osapi.h"
#include "core/CStrings.h"

static constexpr char LINUX_QUERY_VID[] = "udevadm info -a -n %s | grep '{idVendor}' | head -n1";
static constexpr char LINUX_QUERY_PID[] = "udevadm info -a -n %s | grep '{idProduct}' | head -n1";

static void runCommand(char* result, size_t n,const char* c) {
    FILE* fp;
    fp = popen(c, "r");
    if (fp == NULL) {
        throw std::runtime_error("Could not run command.");
    }

    /* Read the output a line at a time - output it. */
    if (fgets(result, n - 1, fp) == NULL) {
        // fgets returned null, so there was no output. Return empty string.
        result[0] = '\0';
    }
    pclose(fp);
}

template<size_t n>
static void runCommand(char (&result)[n], const char* command) { runCommand(result, n, command); }

void core::findUsbSerialPortByProductId(char* devicePath, size_t n, const char* vid, const char* pid) {
    char command[128];
    char cmdOut[128];
    for (uint8_t i = 0; i < 32; i++) {
        snprintf(devicePath, n - 1, "/dev/ttyUSB%i", i);
        if (access(devicePath, F_OK) != 0)
            continue; // device path doesn't exist, try next

        snprintf(command, sizeof(command), LINUX_QUERY_VID, devicePath);
        runCommand(cmdOut, command);
        if (core::cstrings::isNullOrEmpty(cmdOut)
            || !core::cstrings::contains(cmdOut, '\"')) {
            core::cstrings::empty(devicePath);
            return;
        }

        char pidOrVid[5] = {'\0'};
        strncpy(pidOrVid, strchr(cmdOut, '\"') + 1, 4);
        if (!core::cstrings::equalsCaseInvariant(pidOrVid, vid))
            continue;

        snprintf(command, sizeof(command), LINUX_QUERY_PID, devicePath);
        runCommand(cmdOut, command);
        if (core::cstrings::isNullOrEmpty(cmdOut)
            || !core::cstrings::contains(cmdOut, '\"')) {
            core::cstrings::empty(devicePath);
            return;
        }

        strncpy(pidOrVid, strchr(cmdOut, '\"') + 1, 4);
        if (!core::cstrings::equalsCaseInvariant(pidOrVid, pid))
            continue;

        return; // device with current devicePath matches VID and PID.
    }
    core::cstrings::empty(devicePath);
    return;
}

#endif