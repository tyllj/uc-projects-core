//
// Created by tyll on 2022-09-26.
//

#ifdef _WIN32
#include <chrono>
#include <cstdint>
#include <cmath>
#include <windows.h>
#include "hal/tick.h"

static uint64_t appStart = 0;
namespace {
    struct TickInit {
        TickInit() {
            appStart = core::hal::millis();
        }
    } _;
}

void core::hal::sleepms(uint64_t delay) {
    HANDLE timer = CreateWaitableTimer(NULL, FALSE, NULL);
    double seconds = static_cast<double>(delay) * 1e-3;
    thread_local static double estimate = 5e-3;
    thread_local static double mean = 5e-3;
    thread_local static double m2 = 0;
    thread_local static int64_t count = 1;

    while (seconds - estimate > 1e-7) {
        double toWait = seconds - estimate;
        LARGE_INTEGER due;
        due.QuadPart = -int64_t(toWait * 1e7);
        auto start = std::chrono::high_resolution_clock::now();
        SetWaitableTimerEx(timer, &due, 0, NULL, NULL, NULL, 0);
        WaitForSingleObject(timer, INFINITE);
        auto end = std::chrono::high_resolution_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double error = observed - toWait;
        double delta = error - mean;
        mean += delta / count;
        m2   += delta * (error - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    auto start = std::chrono::high_resolution_clock::now();
    while ((std::chrono::high_resolution_clock::now() - start).count() / 1e9 < seconds);
    CloseHandle(timer);
}

uint64_t core::hal::millis() {
    return GetTickCount64() - appStart;
}

#endif