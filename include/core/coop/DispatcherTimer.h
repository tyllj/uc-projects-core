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
    template<typename TFunctor>
    class DispatcherTimer {
    public:
        DispatcherTimer(TFunctor onTick, uint64_t interval) {
            auto timerDelegate = [&last = _last, interval, onTick](){
                if (millisPassedSince(last) >= interval) {
                    last = millis();
                    onTick();
                }
                return yieldContinue();
            };
            _future = core::shared_ptr<IFuture>(new Future<void, decltype(timerDelegate)>(timerDelegate));
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
