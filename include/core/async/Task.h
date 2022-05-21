//
// Created by tyll on 2022-05-09.
//

#ifndef SGLOGGER_TASK_H
#define SGLOGGER_TASK_H

#include "etl/optional.h"
#include "etl/delegate.h"

namespace core { namespace async {

    template<typename TData, typename TResult>
    class Task;

    template<typename TData, typename TResult>
    class TaskContext {
    public:
        TaskContext(TData data, etl::delegate<void(TaskContext<TData, TResult>&)> sliceFunc) :
            _data(data),
            _result(etl::nullopt),
            _completed(false),
            _sliceFunc(sliceFunc) { }

        TData& getData() {
            return _data;
        }
        void setResult(etl::optional<TResult> result) {
            _result = result;
            _completed = true;
        }

    private:
        TData _data;
        etl::optional<TResult> _result;
        bool _completed;
        etl::delegate<void(TaskContext<TData, TResult>&)> _sliceFunc;
        friend class Task<TData, TResult>;
    };

    template<typename TData, typename TResult>
    class Task {
    public:
        Task(TData data, etl::delegate<void(TaskContext<TData, TResult>&)> sliceFunc) :
            _context(data, sliceFunc) { }

        void runSlice() {
            if (!isCompleted())
                _context._sliceFunc(_context);
        }

        void wait() {
            while (!isCompleted())
                _context._sliceFunc(_context);
        }

        Task<TaskContext<TData, TResult>&, TResult> continueWith(etl::delegate<void(TData&, etl::optional<TResult>&)> continuationAction) {
            struct ContinuationContext {
                ContinuationContext(TaskContext<TData, TResult>& parentContext, etl::delegate<void(TData&, etl::optional<TResult>&)> continuationAction)
                    : parentContext(parentContext), continuationAction(continuationAction) {}
                TaskContext<TData, TResult>& parentContext;
                etl::delegate<void(TData&, etl::optional<TResult>&)> continuationAction;
            };

            ContinuationContext continuationContext(_context, continuationAction);

            Task<ContinuationContext, TResult> t(continuationContext,
                                                          [](TaskContext<ContinuationContext, TResult>& ctx){
                                                              TaskContext<TData, TResult>& parentContext(ctx.getData().parentContext);
                                                              etl::delegate<void(TData&, etl::optional<TResult>&)>& continuationAction(ctx.getData().continuationAction);
                                                              if (!parentContext._completed) {
                                                                  parentContext._sliceFunc(parentContext);
                                                              }
                                                              else if (!ctx._completed){
                                                                  ctx.gecontinuationAction(parentContext.getData(), parentContext._result);
                                                                  ctx._result = parentContext._result;
                                                                  ctx._completed = true;
                                                              }
                                                          });
            return t;
        }

        bool isCompleted() {
            return _context._completed;
        }

        etl::optional<TResult>& getResult() {
            return _context.getData();
        }

    private:
        TaskContext<TData, TResult> _context;
    };
}}
#endif //SGLOGGER_TASK_H
