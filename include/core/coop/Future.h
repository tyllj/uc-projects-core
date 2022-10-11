//
// Created by tyll on 2022-05-09.
//

#ifndef UC_CORE_FUTURE_H
#define UC_CORE_FUTURE_H

#include "etl/optional.h"
#include "etl/delegate.h"
#include "core/shared_ptr.h"
#include "core/Tick.h"

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
        template<typename TFuture>
        void run(TFuture future) {
            this->run(core::shared_ptr<IFuture>(reinterpret_cast<IFuture*>(new auto {future})));
        }
    };

    template<typename TResult>
    struct FutureResult {
        FutureResult(etl::optional<TResult> value, bool isCompleted) : value(value), isCompleted(isCompleted) {}

        etl::optional<TResult> value;
        bool isCompleted;
        typedef TResult resultType;
    };

    template<>
    struct FutureResult<void> {
        FutureResult(bool isCompleted) : isCompleted(isCompleted) {}
        bool isCompleted;
        typedef void resultType;
    };

    template<typename TResult>
    FutureResult<TResult> yieldReturn(TResult value) {
        return FutureResult<TResult>(value, true);
    }

    FutureResult<void> yieldReturn() {
        return FutureResult<void>(true);
    }

    template<typename TResult>
    FutureResult<TResult> yieldContinue() {
        return FutureResult<TResult>(etl::nullopt, false);
    }

    FutureResult<void> yieldContinue() {
        return FutureResult<void>(false);
    }

    template<typename TResult, typename TSliceFunctor>
    class Future;

    template<typename TResult, typename TParentSliceFunctor, typename TContinuationFunctor>
    class FutureWithContinuation : public IFuture {
    public:
        FutureWithContinuation(Future<TResult, TParentSliceFunctor> future, TContinuationFunctor continuation) : _future(future), _continuation(continuation), _isCompleted(false) {

        }

        bool runSlice() final {
            if (_isCompleted)
                return true;

            if (_future.runSlice()) {
                _continuation(_future.get());
                _isCompleted = true;
                return true;
            }
            return false;
        }

        void wait() final {
            while (!runSlice());
        }

        bool isCompleted() final {
            return _isCompleted;
        }

        void cancel() final {
            _isCompleted = true;
        }

        shared_ptr<FutureWithContinuation<TResult, TParentSliceFunctor, TContinuationFunctor>> share() {
            return shared_ptr<FutureWithContinuation<TResult, TParentSliceFunctor, TContinuationFunctor>>(new FutureWithContinuation<TResult, TParentSliceFunctor, TContinuationFunctor> { *this });
        }

    private:
        Future<TResult, TParentSliceFunctor> _future;
        TContinuationFunctor _continuation;
        bool _isCompleted;
    };

    template<typename TParentSliceFunctor, typename TContinuationFunctor>
    class FutureWithContinuation<void, TParentSliceFunctor, TContinuationFunctor> : public IFuture {
    public:
        FutureWithContinuation(Future<void, TParentSliceFunctor> future, TContinuationFunctor continuation) : _future(future), _continuation(continuation), _isCompleted(false) {

        }

        bool runSlice() final {
            if (_isCompleted)
                return true;

            if (_future.runSlice()) {
                _continuation();
                _isCompleted = true;
                return true;
            }
            return false;
        }

        void wait() final {
            while (!runSlice());
        }

        bool isCompleted() final {
            return _isCompleted;
        }

        void cancel() final {
            _isCompleted = true;
        }

        shared_ptr<FutureWithContinuation<void, TParentSliceFunctor, TContinuationFunctor>> share() {
            return shared_ptr<FutureWithContinuation<void, TParentSliceFunctor, TContinuationFunctor>>(new FutureWithContinuation<void, TParentSliceFunctor, TContinuationFunctor> { *this });
        }

    private:
        Future<void, TParentSliceFunctor> _future;
        TContinuationFunctor _continuation;
        bool _isCompleted;
    };

    template<typename TResult, typename TSliceFunctor>
    class Future : public IFuture {
    public:
        // TSliceFunctor: FutureResult<TResult>(loopBody)(void)
        Future(TSliceFunctor loopBody) : _functor(loopBody) {}
        Future(TResult result) : _functor([](){}), _isCompleted(true), _result(result) {}

        ~Future() {}

        bool runSlice() final {
            if (_isCompleted)
                return true;

            FutureResult<TResult> f = _functor();
            _isCompleted = f.isCompleted;
            _result = f.value;
            return _isCompleted;
        }

        void wait() final {
            while (!runSlice());
        }

        bool isCompleted() final {
            return _isCompleted;
        }

        void cancel() final {
            _isCompleted = true;
        }

        TResult get() {
            if (!_isCompleted)
                wait();

            return _result.value();
        }

        shared_ptr<Future<TResult, TSliceFunctor>> share() {
            return shared_ptr<Future<TResult, TSliceFunctor>>(new Future<TResult, TSliceFunctor> { *this });
        }

        template<typename TContinuationFunctor>
        FutureWithContinuation<TResult, TSliceFunctor, TContinuationFunctor> continueWith(TContinuationFunctor continuation) {
            return FutureWithContinuation<TResult, TSliceFunctor, TContinuationFunctor>(*this, continuation);
        }

    private:
        TSliceFunctor _functor;
        bool _isCompleted = false;
        etl::optional<TResult> _result = etl::nullopt;
    };

    template<typename TSliceFunctor>
    class Future<void, TSliceFunctor> : public IFuture {
    public:
        // TSliceFunctor: FutureResult<TResult>(loopBody)(void)
        Future(TSliceFunctor loopBody) : _functor(loopBody) {}

        ~Future() {}

        bool runSlice() final {
            if (_isCompleted)
                return true;

            FutureResult<void> f = _functor();
            _isCompleted = f.isCompleted;
            return _isCompleted;
        }

        void wait() final {
            while (!runSlice());
        }

        bool isCompleted() final {
            return _isCompleted;
        }

        void cancel() final {
            _isCompleted = true;
        }

        void get() {
            if (!_isCompleted)
                wait();
        }

        shared_ptr<Future<void, TSliceFunctor>> share() {
            return shared_ptr<Future<void, TSliceFunctor>>(new Future<void, TSliceFunctor> { *this});
        }

        template<typename TContinuationFunctor>
        FutureWithContinuation<void, TSliceFunctor, TContinuationFunctor> continueWith(TContinuationFunctor continuation) {
            return FutureWithContinuation<void, TSliceFunctor, TContinuationFunctor>(*this, continuation);
        }

    private:
        TSliceFunctor _functor;
        bool _isCompleted = false;
    };

    template<typename TSliceFunc>
    auto async(TSliceFunc sliceFunc) {
        using resultType = typename decltype(sliceFunc())::resultType;
        return Future<resultType ,TSliceFunc>(sliceFunc);
    }

    auto delayms(uint64_t millis) {
        return async( [start = core::millis(), delay = millis]()  {
            if (core::millisPassedSince(start) >= delay)
                return yieldReturn();
            return yieldContinue();
        });
    }
}}
#endif //UC_CORE_FUTURE_H
