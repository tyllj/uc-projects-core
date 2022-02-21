//
// Created by tyll on 2022-01-27.
//

#ifndef SGLOGGER_EVENT_H
#define SGLOGGER_EVENT_H

#include <stddef.h>
#include "etl/delegate.h"
#include "etl/list.h"
#include "shared_ptr.h"

#ifndef _CORE_EVENT_SIZE
#define _CORE_EVENT_SIZE (8)
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
            _invokationList.push_back(handler);
        }

        void remove(const etl::delegate<void(TArgs)>& handler) {
            _invokationList.remove(handler);
        }

    protected:
        void invoke(TArgs args) {
            for (etl::delegate<void(TArgs)>& d : _invokationList){
                d(args);
            }
        }


    private:
        etl::list<etl::delegate<void(TArgs)>, _CORE_EVENT_SIZE> _invokationList;
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

        void invoke(TArgs args)  {
            this->Event<TArgs>::invoke(args);
        }
    };
}

#endif //SGLOGGER_EVENT_H
