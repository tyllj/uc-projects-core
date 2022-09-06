//
// Created by tyll on 2022-05-29.
//

#ifndef SGLOGGER_MAINLOOPDISPATCHER_H
#define SGLOGGER_MAINLOOPDISPATCHER_H

#include "etl/vector.h"
#include "Future.h"

namespace core { namespace async {
    template <size_t MAX_TASKS>
    class MainLoopDispatcher : public IDispatcher {
    public:
        void dispatchOne() {
            if (_tasks.empty())
                return;

            if (_current > _tasks.size() - 1)
                _current = 0;

            auto currentTask = _tasks.at(_current).get();
            if (currentTask->runSlice())
                _tasks.erase(_tasks.begin() + _current);

            _current++;
        }

        void run(core::shared_ptr<IFuture> future) final {
            _tasks.push_back(future);
        }

    private:
        etl::vector<core::shared_ptr<IFuture>, MAX_TASKS> _tasks;
        size_t _current;
    };
}}


#endif //SGLOGGER_MAINLOOPDISPATCHER_H
