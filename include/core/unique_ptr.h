//
// Created by tyll on 2022-05-29.
//

#ifndef UC_CORE_UNIQUE_PTR_H
#define UC_CORE_UNIQUE_PTR_H

namespace core {
    template<typename T>
    class unique_ptr {
        T* _ptr;
    public:
        unique_ptr()
                : _ptr(nullptr)
        {}
        // Explicit constructor
        explicit unique_ptr(T* data)
                : _ptr(data)
        {
        }
        ~unique_ptr() {
            delete _ptr;
        }

        // Constructor/Assignment that allows move semantics
        unique_ptr(unique_ptr&& moving) noexcept
                : _ptr(nullptr) {
            moving.swap(*this);
        }
        unique_ptr& operator=(unique_ptr&& moving) noexcept {
            moving.swap(*this);
            return *this;
            // See move constructor.
        }

        // Constructor/Assignment for use with types derived from T
        template<typename U>
        unique_ptr(unique_ptr<U>&& moving) {
            unique_ptr<T>   tmp(moving.release());
            tmp.swap(*this);
        }
        template<typename U>
        unique_ptr& operator=(unique_ptr<U>&& moving) {
            unique_ptr<T>    tmp(moving.release());
            tmp.swap(*this);
            return *this;
        }

        // Remove compiler generated copy semantics.
        unique_ptr(unique_ptr const&) = delete;
        unique_ptr& operator=(unique_ptr const&) = delete;

        // Const correct access owned object
        T* operator->() const {return _ptr;}
        T& operator*() const {return *_ptr;}

        // Access to smart pointer state
        T* get() const {return _ptr;}
        explicit operator bool() const {return _ptr;}

        // Modify object state
        T* release() noexcept {
            T* result = _ptr;
            _ptr = nullptr;
            return result;
        }

        void swap(unique_ptr& other) noexcept {
            T* oldPtr = _ptr;
            _ptr = other._ptr;
            other._ptr = oldPtr;
        }
        void reset() {
            T* tmp = release();
            delete tmp;
        }
    };

    template<typename T>
    class unique_ptr<T[]> {
        T* _ptr;
    public:
        unique_ptr()
                : _ptr(nullptr)
        {}
        // Explicit constructor
        explicit unique_ptr(T* data)
                : _ptr(data)
        {

        }
        ~unique_ptr() {
            delete[] _ptr;
        }

        // Constructor/Assignment that allows move semantics
        unique_ptr(unique_ptr<T[]>&& moving) noexcept
                : _ptr(nullptr) {
            moving.swap(*this);
        }
        unique_ptr<T[]>& operator=(unique_ptr<T[]>&& moving) noexcept {
            moving.swap(*this);
            return *this;
            // See move constructor.
        }

        // Constructor/Assignment for use with types derived from T
        template<typename U>
        unique_ptr(unique_ptr<U[]>&& moving) {
            unique_ptr<T> tmp(moving.release());
            tmp.swap(*this);
        }
        template<typename U>
        unique_ptr& operator=(unique_ptr<U[]>&& moving) {
            unique_ptr<T> tmp(moving.release());
            tmp.swap(*this);
            return *this;
        }

        // Remove compiler generated copy semantics.
        unique_ptr(unique_ptr<T[]> const&) = delete;
        unique_ptr<T[]>& operator=(unique_ptr<T[]> const&) = delete;

        // Const correct access owned object
        T* operator->() const {return _ptr;}
        T& operator*() const {return *_ptr;}

        inline T& operator[](size_t i) const {
            return _ptr[i];
        };

        // Access to smart pointer state
        T* get() const {return _ptr;}
        explicit operator bool() const {return _ptr;}

        // Modify object state
        T* release() noexcept {
            T* result = _ptr;
            _ptr = nullptr;
            return result;
        }

        void swap(unique_ptr<T[]>& other) noexcept {
            T* oldPtr = _ptr;
            _ptr = other._ptr;
            other._ptr = oldPtr;
        }

        void reset() {
            T* tmp = release();
            delete tmp;
        }
    };

    template<typename T>
    void swap(unique_ptr<T>& lhs, unique_ptr<T>& rhs) {
        lhs.swap(rhs);
    }
}

#endif //UC_CORE_UNIQUE_PTR_H
