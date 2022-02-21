//
// Created by tyll on 2022-01-27.
//

#ifndef SGLOGGER_SLEEP_H
#define SGLOGGER_SLEEP_H

#ifdef __AVR__
#include <avr/delay.h>
inline void sleepms(long delay) { _delay_ms(delay); }
#else
#include <unistd.h>
inline void sleepms(long delay) { usleep(delay * 1000); }
#endif


#endif //SGLOGGER_SLEEP_H
