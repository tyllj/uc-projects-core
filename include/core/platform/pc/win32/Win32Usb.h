//
// Created by tyll on 29.09.22.
//

#ifndef UC_CORE_WIN32USB_H
#define UC_CORE_WIN32USB_H
#if defined(_WIN32)

#include <stdexcept>
#include <string.h>
#include <windows.h>
#include <Setupapi.h>

namespace core { namespace platform { namespace pc { namespace usb {
    void findUsbSerialPortByProductId(char* devicePath, size_t n, const char* vid, const char* pid) {
        HDEVINFO DeviceInfoSet = {};
        DWORD DeviceIndex =0;
        SP_DEVINFO_DATA DeviceInfoData = {};
        const char* DevEnum = "USB";
        char ExpectedDeviceId[80] = {0}; //Store hardware id
        BYTE szBuffer[1024] = {0};
        DEVPROPTYPE ulPropertyType = {};
        DWORD dwSize = 0;

        ZeroMemory(devicePath, n);
    
        //create device hardware id
        strcpy_s(ExpectedDeviceId, "vid_");
        strcat_s(ExpectedDeviceId, vid);
        strcpy_s(ExpectedDeviceId, "&pid_");
        strcat_s(ExpectedDeviceId, pid);
        //SetupDiGetClassDevs returns a handle to a device information set
        DeviceInfoSet = SetupDiGetClassDevsA(
                NULL,
                DevEnum,
                NULL,
                DIGCF_ALLCLASSES | DIGCF_PRESENT);
        if (DeviceInfoSet == INVALID_HANDLE_VALUE)
            return;

        DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        //Receive information about an enumerated device
        while (SetupDiEnumDeviceInfo(
                DeviceInfoSet,
                DeviceIndex,
                &DeviceInfoData)) {
            DeviceIndex++;
            //Retrieves a specified Plug and Play device property
            if (SetupDiGetDeviceRegistryProperty (DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID,
                                                  &ulPropertyType, (BYTE*)szBuffer,
                                                  sizeof(szBuffer),   // The size, in bytes
                                                  &dwSize)) {
                HKEY hDeviceRegistryKey;
                //Get the key
                hDeviceRegistryKey = SetupDiOpenDevRegKey(DeviceInfoSet, &DeviceInfoData,DICS_FLAG_GLOBAL, 0,DIREG_DEV, KEY_READ);
                if (hDeviceRegistryKey == INVALID_HANDLE_VALUE) {
                    //DWORD Error = GetLastError();
                    throw std::runtime_error("Could not open registry");
                } else {
                    // Read in the name of the port
                    char pszPortName[32];
                    DWORD dwPortNameSize = sizeof(pszPortName);
                    DWORD dwType = 0;
                    if( (RegQueryValueExA(hDeviceRegistryKey,"PortName", NULL, &dwType, (LPBYTE) pszPortName, &dwSize) == ERROR_SUCCESS) && (dwType == REG_SZ)) {
                        // Check if it really is a com port
                        if( strnicmp( pszPortName, "COM", 3) == 0) {
                            int nPortNr = atoi( pszPortName + 3 );
                            if( nPortNr != 0 ) {
                                strncpy_s(devicePath, n, pszPortName, dwPortNameSize);
                            }
                        }
                    }
                    // Close the key now that we are finished with it
                    RegCloseKey(hDeviceRegistryKey);
                }
            }
        }
        if (DeviceInfoSet) {
            SetupDiDestroyDeviceInfoList(DeviceInfoSet);
        }
    }
}}}}

#endif
#endif //SGLOGGER_WIN32USB_H
