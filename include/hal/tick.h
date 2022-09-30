//
// Created by tyll on 2022-09-25.
//

#ifndef UC_CORE_HAL_TICK_H
#define UC_CORE_HAL_TICK_H

#include <stdint.h>
namespace core { namespace hal {
    void sleepms(uint64_t delay);
    uint64_t millis();
}}
#endif //UC_CORE_HAL_TICK_H
