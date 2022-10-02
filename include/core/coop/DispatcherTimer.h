//
// Created by tyll on 2022-10-01.
//

#ifndef UC_CORE_DISPATCHERTIMER_H
#define UC_CORE_DISPATCHERTIMER_H

#include <stdint.h>
#include "Future.h"
#include "core/Tick.h"
#include "core/shared_ptr.h"

namespace core { namespace coop {
    template<typename TData>
    class DispatcherTimer {
    public:
        DispatcherTimer(void(*onTick)(TData), TData data, uint64_t interval) : _interval(interval), _onTick(onTick), _data(data) {
            _future = core::shared_ptr( reinterpret_cast<IFuture*> (new core::coop::Future<DispatcherTimer<TData>*, void*>(this, [](FutureContext<DispatcherTimer<TData>*, void*>& ctx){
                if (millisPassedSince(ctx.getData()->_last) >= ctx.getData()->_interval) {
                    ctx.getData()->_last = millis();
                    ctx.getData()->_onTick(ctx.getData()->_data);
                }
            })));
        }
        ~DispatcherTimer() {
            _future->cancel();
        }

        operator core::shared_ptr<IFuture>() { return _future; }

    private:
        uint64_t _interval;
        uint64_t _last;
        void (*_onTick)(TData);
        TData _data;
        core::shared_ptr<IFuture> _future;
    };
}}



#endif //UC_CORE_DISPATCHERTIMER_H
