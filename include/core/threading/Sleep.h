//
// Created by tyll on 2022-01-27.
//

#ifndef SGLOGGER_SLEEP_H
#define SGLOGGER_SLEEP_H

#include <unistd.h>
inline void sleepms(long delay) { usleep(delay * 1000); }

#endif //SGLOGGER_SLEEP_H
