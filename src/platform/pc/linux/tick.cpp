//
// Created by tyll on 2022-09-26.
//

#if defined (__linux__)

#include "hal/tick.h"
#include <unistd.h>
#include <time.h>


void core::hal::sleepms(uint64_t delay) {
    usleep(delay * 1000);
}

uint64_t core::hal::millis() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (1000 * ts.tv_sec + ts.tv_nsec / 1000000);
}

#endif