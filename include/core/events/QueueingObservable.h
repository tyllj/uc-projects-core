//
// Created by tyll on 2022-01-28.
//

#ifndef UC_CORE_QUEUEINGOBSERVABLE_H
#define UC_CORE_QUEUEINGOBSERVABLE_H

#include "etl/mutex.h"
#include "etl/queue.h"
#include "core/events/Observable.h"
#include "core/LockGuard.h"

namespace core { namespace events {
        template <typename TData, size_t QueueSize>
        class QueueingObservable : public ::core::events::Observable<TData> {
        public:

            explicit QueueingObservable(Observable<TData>& observable) : _observable(observable) {
                _observable.newData() += EventHandler(*this, enqueue);
            }

            ~QueueingObservable() {
                _observable.newData() -= EventHandler(*this, enqueue);
            }

            void forwardAll() {
                bool anyInQueue = false;
                bool dataAvailable = false;
                TData data;
                do {
                    {
                        threading::LockGuard g(_mtx);
                        if (anyInQueue = !_eventQueue.empty()) {
                            _eventQueue.pop_into(data);
                            dataAvailable = true;
                        }
                    }
                    if (dataAvailable) {
                        dataAvailable = false;
                        this->notify(data);
                    }
                } while (anyInQueue);
            }

        private:
            Observable<TData>& _observable;
            etl::mutex _mtx;
            etl::queue<TData, QueueSize, etl::memory_model::MEMORY_MODEL_SMALL> _eventQueue;

            void enqueue(Observable<TData> sender, TData data) {
                threading::LockGuard g(_mtx);
                _eventQueue.push(data);
            }
        };
    }}

#endif //UC_CORE_QUEUEINGOBSERVABLE_H
