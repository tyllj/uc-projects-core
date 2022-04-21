//
// Created by tyll on 2022-01-21.
//

#ifndef UC_CORE_LOCKGUARD_H
#define UC_CORE_LOCKGUARD_H

#include "etl/mutex.h"
#include "core/Bits.h"
#ifdef __AVR__
#include "core/platform/avr/LockGuard.h"
#else
namespace core {
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
}
#endif

#endif //UC_CORE_LOCKGUARD_H
