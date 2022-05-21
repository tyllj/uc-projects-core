/*
 * uptime.h
 *
 *  Created on: Apr 12, 2022
 *      Author: tyll
 */

#ifndef HAL_UPTIME_H_
#define HAL_UPTIME_H_

#include <stdint.h>

extern uint32_t SystemCoreClock;
inline volatile uint64_t g_millis = 0;

inline void uptimeInit(uint32_t systemCoreClock) {
	SysTick_Config(systemCoreClock / 1000U);
}

inline void uptimeInit() {
	uptimeInit(SystemCoreClock);
}

extern "C" void SysTick_Handler(void);
inline void SysTick_Handler(void) {
	g_millis++;
}

inline uint64_t millis() {
	return g_millis;
}

inline uint64_t millisPassedSince(uint64_t timestamp) {
	return millis() - timestamp;
}


#endif /* HAL_UPTIME_H_ */
