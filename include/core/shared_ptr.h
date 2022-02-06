//
// Created by tyll on 2022-01-26.
//

#ifndef SGLOGGER_SHARED_PTR_H
#define SGLOGGER_SHARED_PTR_H

#include <stdint.h>
#include <stdio.h>


#ifdef __AVR__
#include <avr/interrupt.h>
#include <util/atomic.h>

namespace core {
    class avr_atomic_int8_t {
    public:
        avr_atomic_int8_t (volatile int8_t value) : val(value) {}
        int8_t operator ++()
        {
            int8_t returnValue;
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                returnValue = ++val;
            }
            return returnValue;
        }

        int8_t operator --()
        {
            int8_t returnValue;
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                returnValue = --val;
            }
            return returnValue;
        }
    private:
        volatile int8_t val;
    };
}
#define CORE_ATOMIC_IMPL core::avr_atomic_int8_t
#else
#include "etl/atomic.h"
#define CORE_ATOMIC_IMPL etl::atomic_int32_t
#endif

namespace core {
    template <class T>
    class shared_ptr {
    public:
        explicit shared_ptr( T* ptr ) : _refCount(new CORE_ATOMIC_IMPL(1)), _ptr(ptr) {
            //printf("%x shared_ptr created.\n", _ptr);
        }

        shared_ptr( shared_ptr<T>& ptr ) : _refCount(ptr._refCount), _ptr(ptr._ptr) {
            //printf("%x shared_ptr copied.\n", _ptr);
            (*_refCount)++;
        }

        inline T* get() const {
            return _ptr;
        };

        inline int32_t use_count() const {
            return *_refCount;
        };

        inline bool unique() const {
            return use_count() == 1;
        }

        inline bool isNull() const {
            return _ptr == nullptr;
        }

        inline bool notNull() const {
            return _ptr != nullptr;
        }

        inline T& operator* () const {
            return *(_ptr);
        }

        inline T* operator-> () const {
            return _ptr;
        }

        ~shared_ptr() {
            //printf("%x shared_ptr lost scope.\n", _ptr);
            if (--(*_refCount) == 0) {
                //printf("%x shared_ptr destroyed.\n", _ptr);
                delete _ptr;
                delete _refCount;
            }
        }

    private:
        T* _ptr;
        CORE_ATOMIC_IMPL* _refCount;
    };
}
#endif //SGLOGGER_SHARED_PTR_H
