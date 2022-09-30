//
// Created by tyll on 2022-09-28.
//

#ifndef UC_CORE_TICK_H
#define UC_CORE_TICK_H

#include <stdint.h>
#include "hal/tick.h"

namespace core {
    void sleepms(uint64_t delay) { core::hal::sleepms(delay); }
    uint64_t millis() { return core::hal::millis(); }

    inline uint64_t millisPassedSince(uint64_t timestamp) {
        return core::hal::millis() - timestamp;
    }
}
#endif //UC_CORE_TICK_H
