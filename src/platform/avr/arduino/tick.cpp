//
// Created by tyll on 2022-09-26.
//
#ifdef __AVR__
#ifdef ARDUINO
#include <Arduino.h>
#incldue <stdint.h>
#include "hal/tick.h"
uint64_t core::millis() { return ::millis(); }
#endif
#endif

