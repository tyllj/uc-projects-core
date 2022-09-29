//
// Created by tyll on 2022-05-09.
//

#ifndef SGLOGGER_FUTURE_H
#define SGLOGGER_FUTURE_H

#include "etl/optional.h"
#include "etl/delegate.h"
#include "core/unique_ptr.h"

#define FUTURE_FROM_MEMBER(classname, methodname) core::coop::_futureFromMember<classname, &classname::methodname>(this)

namespace core { namespace coop {
    class IFuture {
    public:
        virtual bool runSlice() = 0;
        virtual void wait() = 0;
        virtual bool isCompleted() = 0;
        virtual void cancel() = 0;
        virtual ~IFuture() = default;
    };

    class IDispatcher {
    public:
        virtual void run(core::shared_ptr<IFuture> future) = 0;
    };

    template<typename TData, typename TResult>
    class Future;

    template<typename TData, typename TResult>
    class FutureContext {
    public:
        FutureContext(TData data, etl::delegate<void(FutureContext<TData, TResult>&)> sliceFunc) :
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

        etl::optional<TResult> getResult() {
            return _result;
        }

        bool isCompleted() {
            return _completed;
        }

    private:
        TData _data;
        etl::optional<TResult> _result;
        bool _completed;
        etl::delegate<void(FutureContext<TData, TResult>&)> _sliceFunc;
        friend class Future<TData, TResult>;
    };

    template<typename TData, typename TResult>
    class Future : public IFuture {
    public:

        Future(TData data, etl::delegate<void(FutureContext<TData, TResult>&)> sliceFunc) :
            _context(data, sliceFunc) { }

        bool runSlice() final {
            if (!isCompleted())
                _context._sliceFunc(_context);
            return isCompleted();
        }

        void wait() final {
            while (!isCompleted())
                _context._sliceFunc(_context);
        }

        void cancel() final {
            _context.setResult(etl::optional<TResult>());
        }

        etl::optional<TResult>& waitForResult() {
            wait();
            return getResult();
        }

        etl::optional<TResult>& getResult() {
            return _context.getData();
        }

        bool isCompleted() final {
            return _context.isCompleted();
        }

        template <typename TContinuationData>
        struct ContinuationContext {
            ContinuationContext(FutureContext<TData, TResult>& parentContext, etl::delegate<void(TContinuationData&, etl::optional<TResult>&)> continuationAction, TContinuationData continuationData)
                    : parentContext(parentContext), continuationAction(continuationAction), continuationData(continuationData) {}
            FutureContext<TData, TResult>& parentContext;
            etl::delegate<void(TContinuationData&, etl::optional<TResult>&)> continuationAction;
            TContinuationData continuationData; // captures all data needed in continuation action.
        };
        template <typename TContinuationData>
        Future<ContinuationContext<TContinuationData>, TResult> continueWith(TContinuationData data, etl::delegate<void(TContinuationData&, etl::optional<TResult>&)> continuationAction) {
            ContinuationContext<TContinuationData> continuationContext(_context, continuationAction, data);
            Future<ContinuationContext<TContinuationData>, TResult> t(continuationContext,
                                                   [](FutureContext<ContinuationContext<TContinuationData>, TResult>& ctx){
                                                       FutureContext<TData, TResult>& parentContext(ctx.getData().parentContext);
                                                       etl::delegate<void(TContinuationData&, etl::optional<TResult>&)> continuationAction(ctx.getData().continuationAction);
                                                       TContinuationData& continuationData(ctx.getData().continuationData);
                                                       if (!parentContext.isCompleted()) {
                                                           parentContext._sliceFunc(parentContext);
                                                       }
                                                       else if (!ctx.isCompleted()){
                                                           continuationAction(continuationData, parentContext._result);
                                                           ctx.setResult(parentContext.getResult());
                                                       }
                                                   });
            return t;
        }

    private:
        FutureContext<TData, TResult> _context;
    };

    template<typename T, void (T::*methodName)(void)>
    static core::shared_ptr<IFuture> _futureFromMember(T* instance) {
        auto backgroundTaskDelegate
                = etl::delegate<void(coop::FutureContext<T*, void*>&)>::create([](coop::FutureContext<T*, void*> ctx) {
                    (ctx.getData()->*methodName)();
                });
        return core::shared_ptr<IFuture>(new Future(instance, backgroundTaskDelegate));
    }

    template<typename TFunctor>
    static core::shared_ptr<IFuture> futureFromFunctor(TFunctor functor) {
        auto backgroundTaskDelegate = etl::delegate<void(coop::FutureContext<TFunctor, void*>&)>::create([](coop::FutureContext<TFunctor, void*> ctx) {
            ctx.getData()();
        });
        return core::shared_ptr<IFuture>(new Future(functor, backgroundTaskDelegate));
    }
}}
#endif //SGLOGGER_FUTURE_H
