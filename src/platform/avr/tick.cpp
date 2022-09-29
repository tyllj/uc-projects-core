//
// Created by tyll on 2022-09-26.
//
#ifdef __AVR__
#include <stdint.h>
#include <avr/delay.h>
#include "hal/tick.h"
void core::sleepms(uint64_t delay) { _delay_ms(delay); }
#endif