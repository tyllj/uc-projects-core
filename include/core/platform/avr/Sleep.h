//
// Created by tyll on 2022-03-24.
//

#ifndef UC_CORE_AVR_SLEEP_H
#define UC_CORE_AVR_SLEEP_H

#ifdef __AVR__
#include <avr/delay.h>
inline void sleepms(long delay) { _delay_ms(delay); }
#endif

#endif //UC_CORE_SLEEP_H
