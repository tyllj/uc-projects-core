/*
 * uptime.h
 *
 *  Created on: Apr 12, 2022
 *      Author: tyll
 */

#ifndef HAL_UPTIME_H_
#define HAL_UPTIME_H_

#include <stdint.h>
#include "clock_config.h"

extern uint32_t SystemCoreClock;
inline volatile uint64_t g_millis = 0;

inline void uptimeInit(uint32_t systemCoreClock) {
	SysTick_Config(systemCoreClock / 1000U);
}

inline void uptimeInit() {
	uptimeInit(SystemCoreClock);
}

inline uint64_t millis() {
	return g_millis;
}

inline void sleepms(uint64_t delay) {
	uint64_t now = millis();
	while (millisPassedSince(now) < delay);
}

#endif /* HAL_UPTIME_H_ */
