//
// Created by tyll on 29.09.22.
//

#ifndef UC_CORE_WIN32USB_H
#define UC_CORE_WIN32USB_H
#if defined(_WIN32)

#include <stdexcept>
#include <string.h>
#include <windows.h>
#include <memory>
#include <Setupapi.h>
#include "core/Convert.h"
#include "core/Error.h"
#include "core/Try.h"
#include "core/Defer.h"

namespace core { namespace platform { namespace pc { namespace usb {
    auto findUsbSerialPortByProductId(char* devicePath, size_t n, const char* vid, const char* pid) -> core::ErrorOr<void> {
        memset(devicePath, 0, n);
        //create device hardware id
        char expectedDeviceId[32] = {0};
        core::StringBuilder(expectedDeviceId)
                .append("vid_")
                .append(vid)
                .append("&pid_")
                .append(pid);

        //SetupDiGetClassDevs returns a handle to a device information set
        auto deviceInfoSet = SetupDiGetClassDevsA(NULL, "USB", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
        VERIFY(deviceInfoSet != INVALID_HANDLE_VALUE, "Could not get device class info for USB.");
        auto closeInfoSet = core::Defer([=] { SetupDiDestroyDeviceInfoList(deviceInfoSet); });

        auto deviceInfoData = SP_DEVINFO_DATA();
        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        //Receive information about an enumerated device
        BYTE propertyBuffer[1024] = {0};
        for (auto i = DWORD{0}; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {
            //Retrieves a specified Plug and Play device property
            auto propertyType = DEVPROPTYPE();
            auto size = DWORD();
            if (SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID,
                                                  &propertyType, (BYTE *) propertyBuffer,
                                                  sizeof(propertyBuffer),   // The size, in bytes
                                                  &size)) {
                auto hDeviceRegistryKey = SetupDiOpenDevRegKey(deviceInfoSet, &deviceInfoData, DICS_FLAG_GLOBAL, 0,
                                                               DIREG_DEV, KEY_READ);
                VERIFY(hDeviceRegistryKey != INVALID_HANDLE_VALUE, "Could not open device registry key.");
                auto closeRegistryKey = core::Defer([=] { RegCloseKey(hDeviceRegistryKey); });

                // Read in the name of the port
                char portName[32];
                auto portNameLength = sizeof(portName);
                auto registryValueType = DWORD();
                if ((RegQueryValueExA(hDeviceRegistryKey, "PortName", NULL, &registryValueType, (LPBYTE) portName,
                                      &size) == ERROR_SUCCESS) && (registryValueType == REG_SZ)) {
                    // Check if it really is a com port
                    if (strnicmp(portName, "COM", 3) == 0 && core::convert::toInt32(portName + 3) != 0) {
                        strncpy_s(devicePath, n, portName, portNameLength);
                        return {};
                    }
                }
            }
        }
        return core::Error(0x005BFA11, "No serial port found for this usb device type.");
    }
}}}}

#endif
#endif //SGLOGGER_WIN32USB_H
