//
// Created by tyll on 2022-05-09.
//

#ifndef UC_CORE_FUTURE_H
#define UC_CORE_FUTURE_H

#include "etl/optional.h"
#include "etl/delegate.h"
#include "core/shared_ptr.h"
#include "core/Tick.h"
#include "core/Math.h"

namespace core { namespace coop {
    class IFuture {
    public:
        virtual auto runSlice() -> bool = 0;
        virtual auto wait() -> void = 0;
        virtual auto isCompleted() const -> bool = 0;
        virtual auto cancel() -> void = 0;
        virtual auto suspendedFor() const -> uint64_t = 0;
        virtual ~IFuture() = default;
    };

    class IDispatcher {
    public:
        virtual auto run(core::shared_ptr<IFuture> future) -> void = 0;
        virtual auto await(core::shared_ptr<IFuture> future) -> void = 0;
    };

    template<typename TResult>
    struct FutureResult {
        FutureResult(etl::optional<TResult> value, bool isCompleted) : value(value), delay(0), isCompleted(isCompleted) {}
        FutureResult(uint64_t delay) : value(etl::nullopt), delay(delay), isCompleted(false) {}
        etl::optional<TResult> value;
        uint64_t delay;
        bool isCompleted;
        typedef TResult resultType;
    };

    template<>
    struct FutureResult<void> {
        FutureResult(bool isCompleted) : delay(0), isCompleted(isCompleted) {}
        FutureResult(uint64_t delay) : delay(delay), isCompleted(false) {}
        uint64_t delay;
        bool isCompleted;
        typedef void resultType;
    };

    template<typename TResult>
    auto yieldReturn(TResult value) -> FutureResult<TResult>{
        return FutureResult<TResult>(value, true);
    }

    auto yieldReturn() ->  FutureResult<void> {
        return FutureResult<void>(true);
    }

    template<typename TResult>
    auto yieldContinue() -> FutureResult<TResult> {
        return FutureResult<TResult>(etl::nullopt, false);
    }

    auto yieldContinue() -> FutureResult<void> {
        return FutureResult<void>(false);
    }

    template<typename TResult>
    auto yieldDelay(uint64_t millis) -> FutureResult<TResult> {
        return FutureResult<TResult>(millis);
    }

    auto yieldDelay(uint64_t millis) ->  FutureResult<void> {
        return FutureResult<void>(millis);
    }

    template<typename TResult, typename TSliceFunctor>
    class Future;

    class FutureBase : public IFuture {
    public:
        FutureBase() : _isCompleted(false), _suspendedUntil(0) {}
        FutureBase(bool isCompleted) : _isCompleted(isCompleted), _suspendedUntil(0) {}

        auto cancel() -> void final {
            _isCompleted = true;
        }

        auto isCompleted() const -> bool final {
            return _isCompleted;
        }

        auto wait() -> void final {
            do {
                if (auto delay = suspendedFor())
                    core::sleepms(delay);
            } while (!runSlice());
        }

    protected:
        auto delay(uint64_t millis) -> void {
            _suspendedUntil = core::millis() + millis;
        }

        auto suspendedUntil() const -> uint64_t {
            return _suspendedUntil;
        }

        auto suspendedForImpl() const -> uint64_t {
            auto timeRemaining = static_cast<int64_t>(suspendedUntil() - core::millis());
            return max(timeRemaining, 0);
        }

    private:
        bool _isCompleted;
        uint64_t _suspendedUntil;
    };

    template<typename TResult, typename TParentSliceFunctor, typename TContinuationFunctor>
    class FutureWithContinuation : public FutureBase {
    public:
        FutureWithContinuation(Future<TResult, TParentSliceFunctor> future, TContinuationFunctor continuation) : _future(future), _continuation(continuation) {

        }

        auto runSlice() -> bool final {
            if (isCompleted())
                return true;

            if (_future.runSlice()) {
                _continuation(_future.get());
                cancel();
                return true;
            }
            return false;
        }

        auto share() const -> shared_ptr<FutureWithContinuation<TResult, TParentSliceFunctor, TContinuationFunctor>> {
            return shared_ptr<FutureWithContinuation<TResult, TParentSliceFunctor, TContinuationFunctor>>(new FutureWithContinuation<TResult, TParentSliceFunctor, TContinuationFunctor> { *this });
        }

       auto runOn(IDispatcher& d) -> decltype(share()) {
            auto shared = share();
            d.run(shared);
            return shared;
        }

        auto awaitOn(IDispatcher& d) -> TResult {
            auto shared = share();
            d.await(shared);
            return shared.get();
        }

        auto suspendedFor() const -> uint64_t final {
            return _future.suspendedFor();
        }

    private:
        Future<TResult, TParentSliceFunctor> _future;
        TContinuationFunctor _continuation;
    };

    template<typename TParentSliceFunctor, typename TContinuationFunctor>
    class FutureWithContinuation<void, TParentSliceFunctor, TContinuationFunctor> : public FutureBase {
    public:
        FutureWithContinuation(Future<void, TParentSliceFunctor> future, TContinuationFunctor continuation) : _future(future), _continuation(continuation) {

        }

        auto runSlice() -> bool final {
            if (isCompleted())
                return true;

            if (_future.runSlice()) {
                _continuation();
                cancel();
                return true;
            }
            return false;
        }

        auto suspendedFor() const -> uint64_t final {
            return _future.suspendedFor();
        }

        auto share() const -> shared_ptr<FutureWithContinuation<void, TParentSliceFunctor, TContinuationFunctor>> {
            return shared_ptr<FutureWithContinuation<void, TParentSliceFunctor, TContinuationFunctor>>(new FutureWithContinuation<void, TParentSliceFunctor, TContinuationFunctor>(*this));
        }

        auto runOn(IDispatcher& d) -> decltype(share()) {
            auto shared = share();
            d.run(shared);
            return shared;
        }

        auto awaitOn(IDispatcher& d) -> void {
            auto shared = share();
            d.await(shared);
        }

    private:
        Future<void, TParentSliceFunctor> _future;
        TContinuationFunctor _continuation;

    };

    template<typename TResult, typename TSliceFunctor>
    class Future : public FutureBase {
    public:
        // TSliceFunctor: FutureResult<TResult>(loopBody)(void)
        Future(TSliceFunctor loopBody) : _functor(loopBody) {}
        Future(TResult result) : FutureBase(true), _functor([](){}), _result(result) {}

        auto runSlice() -> bool final {
            if (isCompleted())
                return true;

            auto f = static_cast<FutureResult<TResult>>(_functor());
            if(f.isCompleted) {
                cancel();
                _result = f.value;
            } else if (f.delay) {
                delay(f.delay);
            }

            return f.isCompleted;
        }

        auto get() -> TResult{
            if (!isCompleted())
                wait();

            return _result.value();
        }

        auto suspendedFor() const -> uint64_t final {
            return suspendedForImpl();
        }

        auto share() -> shared_ptr<Future<TResult, TSliceFunctor>> {
            return shared_ptr<Future<TResult, TSliceFunctor>>(new Future<TResult, TSliceFunctor>(*this));
        }

        auto runOn(IDispatcher& d) -> decltype(share()) {
            auto shared = share();
            d.run(shared);
            return shared;
        }

        auto awaitOn(IDispatcher& d) -> TResult {
            auto shared = share();
            d.await(shared);
            return shared->get();
        }

        template<typename TContinuationFunctor>
        auto continueWith(TContinuationFunctor continuation) -> FutureWithContinuation<TResult, TSliceFunctor, TContinuationFunctor> {
            return FutureWithContinuation<TResult, TSliceFunctor, TContinuationFunctor>(*this, continuation);
        }

    private:
        TSliceFunctor _functor;
        etl::optional<TResult> _result = etl::nullopt;
    };

    template<typename TSliceFunctor>
    class Future<void, TSliceFunctor> : public FutureBase {
    public:
        // TSliceFunctor: FutureResult<TResult>(loopBody)(void)
        Future(TSliceFunctor loopBody) : _functor(loopBody) {}

        auto runSlice() -> bool final {
            if (isCompleted())
                return true;

            auto f = static_cast<FutureResult<void>>(_functor());
            if (f.isCompleted)
                cancel();
            else if (f.delay)
                delay(f.delay);

            return f.isCompleted;
        }

        auto get() -> void {
            if (!isCompleted())
                wait();
        }

        auto suspendedFor() const -> uint64_t final {
            return suspendedForImpl();
        }

        auto share() -> shared_ptr<Future<void, TSliceFunctor>> {
            return shared_ptr<Future<void, TSliceFunctor>>(new Future<void, TSliceFunctor> (*this));
        }

        auto runOn(IDispatcher& d) -> decltype(share()) {
            auto shared = share();
            d.run(shared);
            return shared;
        }

        auto awaitOn(IDispatcher& d) -> void {
            auto shared = share();
            d.await(shared);
        }

        template<typename TContinuationFunctor>
        auto continueWith(TContinuationFunctor continuation) -> FutureWithContinuation<void, TSliceFunctor, TContinuationFunctor> {
            return FutureWithContinuation<void, TSliceFunctor, TContinuationFunctor>(*this, continuation);
        }

    private:
        TSliceFunctor _functor;
    };

    template<typename TSliceFunc>
    auto async(TSliceFunc sliceFunc) {
        using resultType = typename decltype(sliceFunc())::resultType;
        return Future<resultType ,TSliceFunc>(sliceFunc);
    }

    auto delayms(uint64_t millis) {
        return async( [=] () mutable -> FutureResult<void> {
            if (millis) {
                millis = 0;
                return yieldDelay(millis);
            }
            return yieldReturn();
        });
    }
}}
#endif //UC_CORE_FUTURE_H
