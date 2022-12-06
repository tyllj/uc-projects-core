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
    class DispatcherTimer {
    public:
        template<typename TFunctor>
        DispatcherTimer(IDispatcher& dispatcher, uint64_t interval, TFunctor onTick) {
            _future = core::coop::async([&last = _last, interval, onTick](){
                onTick();
                return yieldDelay(interval);
            }).runOn(dispatcher);
        }

        ~DispatcherTimer() {
            _future->cancel();
        }

        operator core::shared_ptr<IFuture>() { return _future; }

    private:
        uint64_t _last = 0;
        core::shared_ptr<IFuture> _future;
    };
}}



#endif //UC_CORE_DISPATCHERTIMER_H
