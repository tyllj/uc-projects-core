//
// Created by tyll on 2022-01-21.
//

#ifndef SGLOGGER_POSIXMUTEX_H
#define SGLOGGER_POSIXMUTEX_H

#include "unistd.h"
#include "pthread.h"
#include "INativeMutex.h"

namespace core { namespace threading {
        class PosixMutex : public INativeMutex{
        public:
            PosixMutex() : _mtx(PTHREAD_MUTEX_INITIALIZER) {

            }
            ~PosixMutex() {
                pthread_mutex_destroy(&_mtx);
            }
            virtual void WaitOne() {
                pthread_mutex_lock(&_mtx);
            };
            virtual void ReleaseMutex() {
                pthread_mutex_unlock(&_mtx);
            };
        private:
            pthread_mutex_t _mtx;
        };
    }}

#endif //SGLOGGER_POSIXMUTEX_H
