//
// Created by tyll on 2022-09-26.
//

#ifdef _WIN32
#include <stdint.h>
#include <windows.h>
#include "hal/tick.h"

void core::hal::sleepms(uint64_t delay) { Sleep(delay); }

uint64_t core::hal::millis() {
    return GetTickCount64();
}

#endif