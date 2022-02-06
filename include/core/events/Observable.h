//
// Created by tyll on 2022-01-28.
//

#ifndef SGLOGGER_OBSERVABLE_H
#define SGLOGGER_OBSERVABLE_H

#include "core/Event.h"
#include "core/EventHandler.h"

namespace core { namespace events {
    template <typename TData>
    class Observable {
    public:
        Event<Observable<TData>&, TData>& newData() { return _newDataEvent; }

    protected:
        void notify(TData data) { _newDataEvent.invoke(*this, data); }

    private:
        InvokableEvent<Observable<TData>&, TData> _newDataEvent;
    };
}}

#endif //SGLOGGER_OBSERVABLE_H
