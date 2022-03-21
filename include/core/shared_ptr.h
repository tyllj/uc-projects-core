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
    class _avr_atomic_int8_t {
    public:
        explicit _avr_atomic_int8_t (volatile int8_t value) : val(value) {}
        operator int32_t() {
            return val;
        }
        operator int8_t() {
            return val;
        }
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

        int8_t operator ++(int) {
            int8_t returnValue;
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                returnValue = val++;
            }
            return returnValue;
        }

        int8_t operator --(int) {
            int8_t returnValue;
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                returnValue = val--;
            }
            return returnValue;
        }
    private:
        volatile int8_t val;
    };
}
#define _CORE_ATOMIC_IMPL core::_avr_atomic_int8_t
#else
#include "etl/atomic.h"
#define _CORE_ATOMIC_IMPL etl::atomic_int8_t
#endif

namespace core {
    template <class T>
    class shared_ptr {
    public:
        constexpr shared_ptr() : _refCount(nullptr), _ptr(nullptr) {}
        explicit shared_ptr(T* const ptr ) : _refCount(new _CORE_ATOMIC_IMPL(1)), _ptr(ptr) {}

        shared_ptr(const shared_ptr<T>& ptr) : _refCount(ptr._refCount), _ptr(ptr._ptr) {
            if (_refCount)
                (*_refCount)++;
        }

        shared_ptr(shared_ptr<T>&& ptr) noexcept : _refCount(ptr._refCount), _ptr(ptr._ptr) {
            ptr._refCount = nullptr;
            ptr._ptr = nullptr;
        }

        ~shared_ptr() {
            if (_refCount && --(*_refCount) == 0) {
                delete _ptr;
                delete _refCount;
            }
        }

        shared_ptr& operator=(const shared_ptr& other) {
            shared_ptr<T>(other).swap(*this);
            return *this;
        }

        void swap(shared_ptr& other) {
            _CORE_ATOMIC_IMPL* oldRefCount = _refCount;
            T* oldPtr = _ptr;
            _refCount = other._refCount;
            _ptr = other._ptr;
            other._refCount = oldRefCount;
            other._ptr = oldPtr;
        }

        void reset() {
            shared_ptr().swap(*this);
        }

        void reset(T* ptr) {
            shared_ptr(ptr).swap(*this);
        }

        inline T* get() const {
            return _ptr;
        };

        inline int32_t use_count() const {
            return (int32_t) *_refCount;
        };

        inline bool unique() const {
            return ((int8_t) *_refCount) == 1;
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

    private:
        T* _ptr;
        _CORE_ATOMIC_IMPL* _refCount;
    };

    template <class T>
    class shared_ptr<T[]> {
    public:
        constexpr shared_ptr() : _refCount(nullptr), _ptr(nullptr) {}
        explicit shared_ptr(T* const ptr) : _refCount(new _CORE_ATOMIC_IMPL(1)), _ptr(ptr) {
        }

        shared_ptr(const shared_ptr<T[]>& ptr) : _refCount(ptr._refCount), _ptr(ptr._ptr) {
            if (_refCount)
                (*_refCount)++;
        }

        shared_ptr(shared_ptr<T[]>&& ptr) noexcept : _refCount(ptr._refCount), _ptr(ptr._ptr) {
            ptr._refCount = nullptr;
            ptr._ptr = nullptr;
        }

        ~shared_ptr() {
            if (_refCount && --(*_refCount) == 0) {
                delete[] _ptr;
                delete _refCount;
            }
        }

        shared_ptr& operator=(const shared_ptr& other) {
            shared_ptr<T[]>(other).swap(*this);
            return *this;
        }

        void swap(shared_ptr<T[]>& other) {
            _CORE_ATOMIC_IMPL* oldRefCount = _refCount;
            T* oldPtr = _ptr;
            _refCount = other._refCount;
            _ptr = other._ptr;
            other._refCount = oldRefCount;
            other._ptr = oldPtr;
        }

        void reset() {
            shared_ptr().swap(*this);
        }

        void reset(T* ptr) {
            shared_ptr(ptr).swap(*this);
        }

        inline T* get() const {
            return _ptr;
        };

        inline int32_t use_count() const {
            return (int32_t) *_refCount;
        };

        inline bool unique() const {
            return ((int8_t) *_refCount) == 1;
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

        inline T& operator[](size_t i) const {
            return _ptr[i];
        };

        inline T* operator-> () const {
            return _ptr;
        }

    private:
        T* _ptr;
        _CORE_ATOMIC_IMPL* _refCount;
    };
}
#endif //SGLOGGER_SHARED_PTR_H
