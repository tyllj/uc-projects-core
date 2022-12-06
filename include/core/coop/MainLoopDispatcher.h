//
// Created by tyll on 2022-05-29.
//

#ifndef UC_CORE_MAINLOOPDISPATCHER_H
#define UC_CORE_MAINLOOPDISPATCHER_H

#include "etl/vector.h"
#include "etl/list.h"
#include "Future.h"

namespace core { namespace coop {
    enum class DispatcherDelayPolicy {
        SLEEP_ON_DELAY,
        YIELD_ON_DELAY
    };

    template <size_t MAX_TASKS, DispatcherDelayPolicy delayPolicy = DispatcherDelayPolicy::YIELD_ON_DELAY>
    class MainLoopDispatcher : public IDispatcher {
    public:
        MainLoopDispatcher() : _current(_tasks.end()) {}

        auto dispatchOne() -> void {
            if (_tasks.empty())
                return;

            auto currentTask = getNextReadyToRun();
            if (currentTask != _tasks.end())
                (*currentTask)->runSlice();
        }

        auto run(core::shared_ptr<IFuture> future) -> void final {
            if (_tasks.available() && !contains(future))
                _tasks.insert(_current, future);
        }

    private:
        typedef typename etl::list<core::shared_ptr<IFuture>, MAX_TASKS>::iterator TaskIterator;

        auto contains(const core::shared_ptr<IFuture>& future) const -> bool {
            for (auto x : _tasks)
                if (x.get() == future.get())
                    return true;
            return false;
        }

        auto getNextReadyToRun() -> TaskIterator {
            auto earliestTask = _tasks.end();
            auto earliestDelay = UINT64_MAX;
            auto currentDelay = uint64_t {};

            for (auto candidates = _tasks.size(); candidates >= 1; --candidates) {
                if (_current == _tasks.end())
                    _current = _tasks.begin();

                if ((*_current)->isCompleted()) {
                    _tasks.erase(_current++);
                    continue;
                }

                currentDelay = (*_current)->suspendedFor();
                if (currentDelay == 0)
                    return _current;

                if (delayPolicy == DispatcherDelayPolicy::SLEEP_ON_DELAY && currentDelay < earliestDelay) {
                    earliestTask = _current;
                    earliestDelay = currentDelay;
                }

                _current++;
            }

            if (delayPolicy == DispatcherDelayPolicy::SLEEP_ON_DELAY && earliestTask != _tasks.end()) {
                core::sleepms(earliestDelay);
                return earliestTask;
            }

            return _tasks.end();
        }

    private:
        etl::list<core::shared_ptr<IFuture>, MAX_TASKS> _tasks;
        TaskIterator _current;
    };
}}


#endif //UC_CORE_MAINLOOPDISPATCHER_H
