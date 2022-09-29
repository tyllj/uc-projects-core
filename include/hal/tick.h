//
// Created by tyll on 2022-09-25.
//

#ifndef SGLOGGER_HAL_TICK_H
#define SGLOGGER_HAL_TICK_H

#include <stdint.h>
namespace core { namespace hal {
    void sleepms(uint64_t delay);
    uint64_t millis();
}}
#endif //SGLOGGER_HAL_TICK_H
