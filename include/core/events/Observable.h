//
// Created by tyll on 2022-01-28.
//

#ifndef UC_CORE_OBSERVABLE_H
#define UC_CORE_OBSERVABLE_H

#include "core/Event.h"

namespace core { namespace events {
    template <typename TData>
    class Observable {
    public:
        Event<TData>& newData() { return _newDataEvent; }

    protected:
        void notify(TData data) {
            _newDataEvent.invoke(data);
        }

    private:
        InvokableEvent<TData> _newDataEvent;
    };
}}

#endif //UC_CORE_OBSERVABLE_H
