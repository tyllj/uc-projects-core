//
// Created by tyll on 2022-01-21.
//

#ifndef SGLOGGER_LOCKGUARD_H
#define SGLOGGER_LOCKGUARD_H

#include "etl/mutex.h"

namespace core { namespace threading {
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


#endif //SGLOGGER_LOCKGUARD_H
