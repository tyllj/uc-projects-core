//
// Created by tyll on 2022-03-19.
//

#ifndef UC_CORE_AVR_LOCKGUARD_H
#define UC_CORE_AVR_LOCKGUARD_H

#ifdef __AVR__

// Just disable interrupts for AVR systems without RTOS, since no context switching occurs. Without preemptive scheduling mutexes would lead to deadlocks.
#include "core/Bits.h"
#include "avr/interrupt.h"
namespace core {
    class LockGuard {
    public:
        LockGuard(etl::mutex& mutexImpl) {
            if (_interruptState = bitRead(SREG, 7))
                cli();
        }

        ~LockGuard() {
            if (_interruptState)
                sei();
        }

        bool _interruptState;
    };
}

#endif

#endif //UC_CORE_LOCKGUARD_H
