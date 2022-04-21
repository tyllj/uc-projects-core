//
// Created by tyll on 2022-01-27.
//

#ifndef UC_CORE_EVENT_H
#define UC_CORE_EVENT_H

#include <stddef.h>
#include "etl/delegate.h"
#include "etl/list.h"
#include "shared_ptr.h"

#ifndef CORE_EVENT_SIZE
#define CORE_EVENT_SIZE (8)
#endif

namespace core {
    template<class TArgs>
    class Event {
    public:
        Event<TArgs>& operator += (const etl::delegate<void(TArgs)>& handler) {
            this->add(handler);
            return *this;
        }

        Event<TArgs>& operator -=(const etl::delegate<void(TArgs)>& handler) {
            this->remove(handler);
            return *this;
        }

        void add(const etl::delegate<void(TArgs)>& handler) {
            _invocationList.push_back(handler);
        }

        void remove(const etl::delegate<void(TArgs)>& handler) {
            _invocationList.remove(handler);
        }

    protected:
        void invoke(TArgs args) {
            for (etl::delegate<void(TArgs)>& d : _invocationList){
                d(args);
            }
        }

        bool hasListeners() {
            return this->_invocationList.empty();
        }


    private:
        etl::list<etl::delegate<void(TArgs)>, CORE_EVENT_SIZE> _invocationList;
    };

    template<class TArgs>
    class InvokableEvent : public Event<TArgs> {
    public:
        Event<TArgs>& operator += (const etl::delegate<void(TArgs)>& handler) {
            this->Event<TArgs>::add(handler);
            return *this;
        }

        Event<TArgs>& operator -=(const etl::delegate<void(TArgs)>& handler) {
            this->Event<TArgs>::remove(handler);
            return *this;
        }

        void invoke(TArgs args) {
            this->Event<TArgs>::invoke(args);
        }

        bool hasListeners() {
            return hasListeners();
        }
    };
}

#endif //UC_CORE_EVENT_H
