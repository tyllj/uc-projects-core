//
// Created by tyll on 2022-03-24.
//

#ifndef UC_CORE_PC_SLEEP_H
#define UC_CORE_PC_SLEEP_H

#include <unistd.h>
inline void sleepms(long delay) { usleep(delay * 1000); }

#endif //UC_CORE_SLEEP_H
