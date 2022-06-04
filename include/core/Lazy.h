//
// Created by tyll on 2022-05-22.
//

#ifndef SGLOGGER_LAZY_H
#define SGLOGGER_LAZY_H

#include "etl/optional.h"
#include "etl/delegate.h"

namespace core {
    template <typename T>
    class Lazy {
    public:
        explicit Lazy(etl::delegate<T(void)> factory) : _factory(factory), _value() {}
        T& get() {
            if (!_value.has_value())
                 _value = _factory();
            return _value;
        }

        void reset() {
            _value.reset();
        }
    private:
        etl::delegate<T(void)> _factory;
        etl::optional<T> _value;
    };
}

#endif //SGLOGGER_LAZY_H
