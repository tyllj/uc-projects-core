//
// Created by tyll on 2022-05-09.
//

#ifndef UC_CORE_FUTURE_H
#define UC_CORE_FUTURE_H

#include "etl/utility.h"
#include "etl/optional.h"
#include "etl/delegate.h"
#include "etl/utility.h"
#include "core/shared_ptr.h"
#include "core/Tick.h"
#include "core/Math.h"
#include "Nothing.h"

namespace core {
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
        virtual auto run(shared_ptr<IFuture> future) -> void = 0;
        virtual auto await(shared_ptr<IFuture> future) -> void = 0;
    };

    template<typename TResult>
    struct FutureResult {
        static auto Completed(TResult result) -> FutureResult<TResult> { return FutureResult(etl::optional<TResult>(result)); }
        static auto Completed(TResult&& result) -> FutureResult<TResult> { return FutureResult(etl::optional<TResult>(result)); }
        static auto Incomplete() -> FutureResult<TResult> { return FutureResult(etl::optional<TResult>(etl::nullopt)); }
        static auto Delayed(uint64_t delay) -> FutureResult<TResult> { return FutureResult(delay); }
        etl::optional<TResult> value;
        uint64_t delay;
        typedef TResult ResultType;
    protected:
        explicit FutureResult(etl::optional<TResult> value) : value(value), delay(0) {}
        explicit FutureResult(uint64_t delay) : value(etl::nullopt), delay(delay) {}
    };

    template<typename TResult>
    auto yieldReturn(TResult value) -> FutureResult<TResult>{
        return FutureResult<TResult>::Completed(value);
    }

    template<typename TResult>
    auto yieldReturn(TResult&& value) -> FutureResult<TResult>{
        return FutureResult<TResult>::Completed(value);
    }

    auto yieldReturn() -> FutureResult<Nothing> {
        return FutureResult<Nothing>::Completed(nothing);
    }

    template<typename TResult>
    auto yieldContinue() -> FutureResult<TResult> {
        return FutureResult<TResult>::Incomplete();
    }

    auto yieldContinue() -> FutureResult<Nothing> {
        return FutureResult<Nothing>::Incomplete();
    }

    template<typename TResult>
    auto yieldDelay(uint64_t millis) -> FutureResult<TResult> {
        return FutureResult<TResult>::Delayed(millis);
    }

    auto yieldDelay(uint64_t millis) ->  FutureResult<Nothing> {
        return FutureResult<Nothing>::Delayed(millis);
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
                    sleepms(delay);
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
            auto timeRemaining = static_cast<int64_t>(suspendedUntil() - millis());
            return max(timeRemaining, 0);
        }

    private:
        bool _isCompleted;
        uint64_t _suspendedUntil;
    };

    template<typename TResult, typename TParentSliceFunctor, typename TContinuationFunctor>
    class FutureWithContinuation : public FutureBase {
    public:
        FutureWithContinuation(Future<TResult, TParentSliceFunctor> future, TContinuationFunctor continuation) : _future(future), _continuation(continuation) {}
        FutureWithContinuation(Future<TResult, TParentSliceFunctor> future, TContinuationFunctor&& continuation) : _future(future), _continuation(continuation) {}

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

       auto runOn(core::IDispatcher& d) -> decltype(share()) {
            auto shared = share();
            d.run(shared);
            return shared;
        }

        auto awaitOn(core::IDispatcher& d) -> TResult {
            auto shared = share();
            d.await(shared);
            return shared.get();
        }

        auto suspendedFor() const -> uint64_t final {
            return _future.suspendedFor();
        }

        typedef TResult ResultType;

    private:
        Future<TResult, TParentSliceFunctor> _future;
        TContinuationFunctor _continuation;
    };

    template<typename TParentSliceFunctor, typename TContinuationFunctor>
    class FutureWithContinuation<void, TParentSliceFunctor, TContinuationFunctor> : public FutureWithContinuation<Nothing, TParentSliceFunctor, TContinuationFunctor> {
    public:
        FutureWithContinuation(Future<void, TParentSliceFunctor> future, TContinuationFunctor continuation) :
                FutureWithContinuation<Nothing, TParentSliceFunctor, TContinuationFunctor>(future, continuation) {}
        FutureWithContinuation(Future<void, TParentSliceFunctor>&& future, TContinuationFunctor&& continuation) :
                FutureWithContinuation<Nothing, TParentSliceFunctor, TContinuationFunctor>(future, continuation) {}
    };

    template<typename TResult, typename TSliceFunctor>
    class Future : public FutureBase {
    public:
        // TSliceFunctor: FutureResult<TResult>(loopBody)(void)
        Future(const TSliceFunctor& loopBody) : _functor(loopBody) {}
        Future(TSliceFunctor&& loopBody) : _functor(etl::move(loopBody)) {}
        Future(TResult result) : FutureBase(true), _functor([](){}), _result(result) {}

        auto runSlice() -> bool final {
            if (isCompleted())
                return true;

            auto f = static_cast<FutureResult<TResult>>(_functor());
            if(f.value.has_value()) {
                cancel();
                _result = f.value;
            } else if (f.delay) {
                delay(f.delay);
            }

            return f.value.has_value();
        }

        auto get() -> TResult {
            if (!isCompleted())
                wait();

            return _result.value();
        }

        auto suspendedFor() const -> uint64_t final {
            return suspendedForImpl();
        }

        auto share() -> shared_ptr<Future<TResult, TSliceFunctor>> {
            return shared_ptr<Future<TResult, TSliceFunctor>>(new Future<TResult, TSliceFunctor>(etl::move(*this)));
        }

        auto runOn(core::IDispatcher& d) -> decltype(share()) {
            auto shared = share();
            d.run(shared);
            return shared;
        }

        auto awaitOn(core::IDispatcher& d) -> TResult {
            auto shared = share();
            d.await(shared);
            return shared->get();
        }

        template<typename TContinuationFunctor>
        auto continueWith(TContinuationFunctor continuation) -> FutureWithContinuation<TResult, TSliceFunctor, TContinuationFunctor> {
            return FutureWithContinuation<TResult, TSliceFunctor, TContinuationFunctor>(*this, continuation);
        }

        typedef TResult ResultType;

    private:
        TSliceFunctor _functor;
        etl::optional<TResult> _result = etl::nullopt;
    };

    template<typename TSliceFunctor>
    class Future<void, TSliceFunctor> : public Future<Nothing, TSliceFunctor> {
    public:
        Future(TSliceFunctor&& loopBody) : Future<Nothing, TSliceFunctor>([functor = loopBody]() -> Nothing { functor(); return {};}) {}

        typedef void ResultType;
    };

    template<typename TSliceFunctor>
    Future(TSliceFunctor&& loopBody) -> Future<typename decltype(loopBody())::ResultType, TSliceFunctor>;

    template<typename TSliceFunctor>
    auto async(TSliceFunctor&& sliceFunc) -> Future<typename decltype(sliceFunc())::ResultType, TSliceFunctor> {
        return Future(etl::move(sliceFunc));
    }

    auto delayms(uint64_t millis) {
        return async([=] () mutable -> FutureResult<Nothing> {
            if (millis) {
                millis = 0;
                return yieldDelay(millis);
            }
            return yieldReturn();
        });
    }
}
#endif //UC_CORE_FUTURE_H
