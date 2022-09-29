//
// Created by tyll on 2022-09-28.
//

#include "hal/osapi.h"

void __attribute__((weak)) core::findUsbSerialPortByProductId(char* devicePath, size_t n, const char* vid, const char* pid) {
    devicePath[0] = '\0';
}