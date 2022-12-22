//
// Created by tu.jungke on 22.12.2022.
//

#ifndef __DEFER_H__
#define __DEFER_H__

#include "etl/memory.h"

namespace core {
    template <typename TeardownFunctor>
    class Defer {
    public:
        Defer(const Defer&) = delete;
        Defer(Defer&& other) : _finalizer(other._finalizer), _owns(other._owns) { other._owns = false; }
        Defer(TeardownFunctor&& finalizer) : _finalizer(finalizer), _owns(true) {}
        ~Defer() {
            if (_owns)
                _finalizer();
        }

    private:
        TeardownFunctor _finalizer;
        bool _owns;
    };
}
#endif //__DEFER_H__
