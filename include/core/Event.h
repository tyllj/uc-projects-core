//
// Created by tyll on 2022-01-27.
//

#ifndef SGLOGGER_EVENT_H
#define SGLOGGER_EVENT_H

#include <stddef.h>
#include "EventHandler.h"
#include "shared_ptr.h"

namespace core {
    template<class TSender, class TArgs>
    class Event {
    public:
        Event& operator += (shared_ptr<IEventHandler<TSender, TArgs>> handler) {
            this->add(handler);
            return *this;
        }

        Event& operator -=(shared_ptr<IEventHandler<TSender, TArgs>> handler) {
            this->remove(handler);
            return *this;
        }

        ~Event() {
            InvocationListItem* current = _first;
            InvocationListItem* next = nullptr;
            while (current != nullptr) {
                next = current->next;
                delete current;
                current = next;
            }
        }

        void add(shared_ptr<IEventHandler<TSender, TArgs>> handler) {
            if (_first == nullptr)
                _first = new InvocationListItem(handler);
            else
                findLast()->next = new InvocationListItem(handler);
        }

        void remove(shared_ptr<IEventHandler<TSender, TArgs>> handler) {
            if (_first == nullptr)
                return;

            InvocationListItem** currentRef = &_first;
            while ((*currentRef) != nullptr) {
                if ((*currentRef)->delegate.get() == handler.get()) {
                    InvocationListItem* next = (*currentRef)->next;
                    delete *currentRef;
                    *currentRef = next;
                    break;
                }
                currentRef = &((*currentRef)->next);
            }
        }

    protected:
        void invoke(TSender sender, TArgs args) {
            InvocationListItem* current = _first;
            while (current != nullptr) {
                *(current->delegate)(sender, args);
                current = current->next;
            }
        }


    private:
        class InvocationListItem {
        public:
            explicit InvocationListItem(shared_ptr<IEventHandler<TSender, TArgs>> handler) : delegate(handler), next(nullptr) {}
            shared_ptr<IEventHandler<TSender, TArgs>> delegate;
            InvocationListItem* next;
        };

        InvocationListItem* _first;

        InvocationListItem* findLast() {
            InvocationListItem* current = _first;
            while (current->next != nullptr)
                current = current->next;
            return current;
        }
    };

    template<class TSender, class TArgs>
    class InvokableEvent : public Event<TSender, TArgs> {
    public:
        InvokableEvent& operator +=(shared_ptr<IEventHandler<TSender, TArgs>> handler) {
            this->add(handler);
            return *this;
        }

        InvokableEvent& operator -=(shared_ptr<IEventHandler<TSender, TArgs>> handler) {
            this->remove(handler);
            return *this;
        }

        void invoke(TSender sender, TArgs args) {
            this->Event<TSender, TArgs>::invoke(sender, args);
        }
    };

    template <typename TSender, typename TListener, typename TEventArgs>
    shared_ptr<IEventHandler<TSender, TEventArgs>> makeEventHandler(TListener& object_, void(TListener::* p_function_)(TSender, TEventArgs)) {
        return shared_ptr<IEventHandler<TSender, TEventArgs>>((IEventHandler<TSender, TEventArgs>*) new EventHandler<TSender, TListener, TEventArgs>(object_, p_function_));
    }
}

#endif //SGLOGGER_EVENT_H
