//
// Created by tyll on 2022-01-21.
//

#ifndef SGLOGGER_LOCKGUARD_H
#define SGLOGGER_LOCKGUARD_H

namespace core { namespace threading {
    template <typename TMutexImpl>
    class LockGuard {
    public:
        LockGuard(TMutexImpl& mutexImpl) : _mutexImpl(mutexImpl) {}
        LockGuard() {
            _mutexImpl.WaitOne();
        }

        ~LockGuard() {
            _mutexImpl.ReleaseMutex();
        }

    private:
        TMutexImpl& _mutexImpl;
    };
}}


#endif //SGLOGGER_LOCKGUARD_H
