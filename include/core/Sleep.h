//
// Created by tyll on 2022-01-27.
//

#ifndef UC_CORE_SLEEP_H
#define UC_CORE_SLEEP_H

#ifdef __AVR__
#include "core/platform/avr/Sleep.h"
#else
#include "core/platform/pc/Sleep.h"
#endif


#endif //UC_CORE_SLEEP_H
