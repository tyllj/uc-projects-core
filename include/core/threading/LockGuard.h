//
// Created by tyll on 2022-01-21.
//

#ifndef SGLOGGER_LOCKGUARD_H
#define SGLOGGER_LOCKGUARD_H

#include "etl/mutex.h"
#include "core/Bits.h"

namespace core { namespace threading {
#ifdef __AVR__

// Just disable interrupts for AVR systems without RTOS, since no context switching occurs. Without preemptive scheduling mutexes would lead to deadlocks.
#include "core/Bits.h"
#include "avr/interrupt.h"
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
#else
    class LockGuard {
    public:
        LockGuard(etl::mutex& mutexImpl) : _mutexImpl(mutexImpl) {
            _mutexImpl.lock();
        }

        ~LockGuard() {
            _mutexImpl.unlock();
        }

    private:
        etl::mutex& _mutexImpl;
    };
}}
#endif

#endif //SGLOGGER_LOCKGUARD_H
