//
// Created by tyll on 2022-03-24.
//

#ifndef UC_CORE_PC_SLEEP_H
#define UC_CORE_PC_SLEEP_H

#include <unistd.h>
#ifdef _WIN32
#include <windows.h>
#elif defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <time.h>
#endif

inline void sleepms(long delay) { usleep(delay * 1000); }

inline uint64_t millis() {
#ifdef _WIN32
    return GetTickCount();
#elif defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (1000 * ts.tv_sec + ts.tv_nsec / 1000000);
#endif
}
#endif //UC_CORE_SLEEP_H
