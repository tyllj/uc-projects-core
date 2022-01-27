//
// Created by tyll on 2022-01-26.
//

#ifndef SGLOGGER_SHARED_PTR_H
#define SGLOGGER_SHARED_PTR_H

#include <stdint.h>
#include <stdio.h>
namespace core {
    template <class T>
    class shared_ptr {
    public:
        explicit shared_ptr( T* ptr ) : _refCount(new volatile int32_t(1)), _ptr(ptr) {
            //printf("%x shared_ptr created.\n", _ptr);
        }

        shared_ptr( shared_ptr<T>& ptr ) : _refCount(ptr._refCount), _ptr(ptr._ptr) {
            //printf("%x shared_ptr copied.\n", _ptr);
            (*_refCount)++;
        }

        T* get() const {
            return _ptr;
        };
        int32_t use_count() const {
            return *_refCount;
        };

        bool unique() const {
            return use_count() == 1;
        }

        T& operator* () const {
            return *(_ptr);
        }

        T* operator-> () const {
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
        T* const _ptr;
        volatile int32_t* const _refCount;
    };
}
#endif //SGLOGGER_SHARED_PTR_H
