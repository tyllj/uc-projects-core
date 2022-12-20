//
// Created by tu.jungke on 15.12.2022.
//

#ifndef __ERROR_H__
#define __ERROR_H__

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
        Error(Error&&) = default;

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

    auto terminateOnUnhandledError(Error const& e) {
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
        ErrorOr(Error&& error) : _result(etl::move(error)) {}
        template <typename TErrorCode>
        ErrorOr(TErrorCode code) : _result(Error(static_cast<int32_t>(code))) {}

        auto value() -> TResult const& {
            if (is_error()) [[unlikely]]
                terminateOnUnhandledError(error());
            return _result.value();
        }

        auto release_value() -> TResult {
            if (is_error()) [[unlikely]]
                terminateOnUnhandledError(error());
            return etl::move(_result.value());
        }

        auto error() -> Error const& {
            if (!is_error()) [[unlikely]] {
                auto e = Error(0x0BADC0DE, "error() evaluated on successful result.");
                terminateOnUnhandledError(e);
            }
            return _result.error();
        }

        auto is_error() -> bool {
            return _result.is_error();
        }

        auto is_success() -> bool {
            return !_result.is_error();
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

        ErrorOr() : ErrorOr<Nothing>({}) {}
        ErrorOr(Error const& error) : ErrorOr<Nothing>(error) {}
        ErrorOr(Error&& error) : ErrorOr<Nothing>(etl::move(error)) {}
        template <typename TErrorCode>
        ErrorOr(TErrorCode code) : ErrorOr<Nothing>(Error(static_cast<int32_t>(code))) {}

        typedef void ResultType;
    };

#define DEFINE_CORE_ERROR(name, errorCode, errorMsg) \
                                                     \
    template<size_t n> static constexpr core::Error name(const char (&msg)[n]) { return core::Error(errorCode, msg); } \
    static constexpr core::Error name() { return core::Error(errorCode, #name "(): " errorMsg); }

    DEFINE_CORE_ERROR(NotImplementedError, 0xDEADC0DE, "Function is not implemented.");
    DEFINE_CORE_ERROR(InvalidArgumentError, 0x00BADA86, "An invalid argument was specified.");


}

#endif //__ERROR_H__
