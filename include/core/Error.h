//
// Created by tu.jungke on 15.12.2022.
//

#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "core/Nothing.h"
#include "etl/result.h"
#include "etl/optional.h"

#ifndef CORE_NO_ERROR_MESSAGES
#define CORE_ERROR_MESSAGES
#endif

// Inspired by SerenityOS error handling: https://github.com/SerenityOS/serenity/blob/master/AK/Error.h
namespace core {
    class Error {
    public:
        Error(Error const& other) : _code(other._code)
#if defined(CORE_ERROR_MESSAGES)
            , _errorMessageLiteral(other._errorMessageLiteral)
#endif
        {}

        Error(Error&& other) : _code(other._code)
#if defined(CORE_ERROR_MESSAGES)
                , _errorMessageLiteral(other._errorMessageLiteral)
#endif
        {}

        template<size_t n>
        constexpr Error(int32_t code, const char (&message)[n]) : _code(code)
#if defined(CORE_ERROR_MESSAGES)
            , _errorMessageLiteral(message)
#endif
        {}
        constexpr Error(int32_t code) : _code(code)
#if defined(CORE_ERROR_MESSAGES)
            , _errorMessageLiteral("No error message.")
#endif
        {}

        template <typename TErrorCode, size_t n>
        constexpr Error(TErrorCode code, const char (&message)[n]) : _code(static_cast<int32_t>(code))
#if defined(CORE_ERROR_MESSAGES)
                , _errorMessageLiteral(message)
#endif
        {}

        template <typename TErrorCode>
        constexpr Error(TErrorCode code) : _code(static_cast<int32_t>(code))
#if defined(CORE_ERROR_MESSAGES)
            , _errorMessageLiteral("No error message.")
#endif
        {}

        auto error_code() const -> int32_t { return _code; }
#if defined(CORE_ERROR_MESSAGES)
        auto what() const -> const char * { return _errorMessageLiteral; }
#endif

    private:
        int32_t _code;
#if defined(CORE_ERROR_MESSAGES)
        const char* _errorMessageLiteral;
#endif
    };

    auto terminateOnUnhandledError(Error const& e) -> void {
#if defined(CORE_ERROR_MESSAGES)
        printf("\nUnhandled error: error_code(): %x (%i), what(): %s\n", e.error_code(), e.error_code(), e.what());
#else
        printf("\nUnhandled error: error_code(): %x (%i)\n", e.error_code(), e.error_code());
#endif
        abort();
    }

    template<typename TResult>
    class ErrorOr  {
    public:
        ErrorOr(ErrorOr const&) = default;
        ErrorOr(ErrorOr&&) = default;

        ErrorOr(TResult const& result) : _result(result) {}
        ErrorOr(TResult&& result) : _result(etl::move(result)) {}
        ErrorOr(Error const& error) : _result(error) {}
        ErrorOr(Error&& error) : _result(error) {}
        template <typename TErrorCode>
        ErrorOr(TErrorCode code) : _result(Error(static_cast<int32_t>(code))) {}

        auto value() const -> TResult const& {
            if (is_error()) [[unlikely]]
                terminateOnUnhandledError(error());

            return _result.value();
        }

        auto release_value() -> TResult {
            if (is_error()) [[unlikely]]
                terminateOnUnhandledError(error());
            return etl::move(_result.value());
        }

        auto error() const -> Error const& {
            if (!is_error()) [[unlikely]] {
                auto e = Error(0x0BADC0DE, "error() evaluated on successful result.");
                terminateOnUnhandledError(e);
            }
            return _result.error();
        }

        auto is_error() const -> bool {
            return _result.is_error();
        }

        auto is_success() const-> bool {
            return !_result.is_error();
        }

        auto operator* () const -> TResult const&{
            return value();
        }

        auto operator-> () const -> TResult const* {
            return &(value());
        }

        typedef TResult ResultType;

    private:
        etl::result<TResult, Error> _result;
    };

    template<>
    class ErrorOr<void> : public ErrorOr<Nothing>{
    public:
        ErrorOr(ErrorOr const&) = default;
        ErrorOr(ErrorOr&&) = default;
        ErrorOr& operator=(ErrorOr&&) = default;
        ErrorOr& operator=(ErrorOr const&) = default;

        ErrorOr() : ErrorOr<Nothing>({}) {}
        ErrorOr(Error const& error) : ErrorOr<Nothing>(error) {}
        ErrorOr(Error&& error) : ErrorOr<Nothing>(etl::move(error)) {}
        //template <typename TErrorCode>
        //ErrorOr(TErrorCode code) : ErrorOr<Nothing>(Error(static_cast<int32_t>(code))) {}

        typedef void ResultType;
    };

#define DEFINE_CORE_ERROR(name, errorCode, errorMsg) \
                                                     \
    template<size_t n> static constexpr core::Error name(const char (&msg)[n]) { return core::Error(errorCode, msg); } \
    static constexpr core::Error name() { return core::Error(errorCode, #name "(): " errorMsg); }

    DEFINE_CORE_ERROR(NotImplementedError, 0xDEADC0DE, "Function is not implemented.");
    DEFINE_CORE_ERROR(InvalidArgumentError, 0x00BADA86, "An invalid argument was specified.");
    DEFINE_CORE_ERROR(NullPointerError, 0x0D15EA5E, "A nullptr was found.");
    DEFINE_CORE_ERROR(AssertionError, 0xDEFEC8ED, "An assertion failed.");

    template<size_t n = sizeof("An assertion failed.")>
    inline static auto verify(bool condition, const char (&errorMsg)[n] = "An assertion failed.") -> ErrorOr<void> {
        if (!condition) [[unlikely]] {
            return AssertionError(errorMsg);
        }
        return {};
    }

    template<size_t n = sizeof("An assertion failed.")>
    inline static auto enforce(bool condition, const char (&errorMsg)[n] = "An assertion failed.") -> void {
        auto v = verify(condition, errorMsg);
        if (v.is_error())
            terminateOnUnhandledError(v.error());
    }

    template<typename T, size_t n = sizeof("A nullptr was found.")>
    inline static auto verifyNotNull(T* ptr, const char (&errorMsg)[n] = "A nullptr was found.") -> ErrorOr<T*> {
        if (ptr == nullptr) [[unlikely]] {
            return NullPointerError(errorMsg);

        }
        return ptr;
    }

    template<typename TPtr, size_t n = sizeof("A nullptr was found.")>
    inline static auto verifyNotNull(TPtr ptr, const char (&errorMsg)[n] = "A nullptr was found.") -> ErrorOr<TPtr> {
        if (static_cast<bool>(ptr) == false) [[unlikely]] {
            return NullPointerError(errorMsg);
        }
        return ptr;
    }

    template<typename T, size_t n = sizeof("A nullptr was found.")>
    inline static auto enforceNotNull(T* ptr, const char (&errorMsg)[n] = "A nullptr was found.") -> T* {
        auto v = verifyNotNull(ptr, errorMsg);
        if (v.is_error())
            terminateOnUnhandledError(v.error());
        return ptr;
    }

    template<typename TPtr, size_t n = sizeof("A nullptr was found.")>
    inline static auto enforceNotNull(TPtr ptr, const char (&errorMsg)[n] = "A nullptr was found.") -> TPtr&& {
        auto v = verifyNotNull(ptr, errorMsg);
        if (v.is_error())
            terminateOnUnhandledError(v.error());
        return etl::move(ptr);
    }



}

#endif //__ERROR_H__
