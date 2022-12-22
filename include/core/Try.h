//
// Created by tu.jungke on 16.12.2022.
//

#ifndef __TRY_H__
#define __TRY_H__

#include "core/Error.h"
#include "core/StringBuilder.h"

// Inspired by SerenityOS error handling: https://github.com/SerenityOS/serenity/blob/master/AK/Try.h


#define TRY(expression)                                   \
    ({                                                    \
        auto _temporary_result = (expression);            \
        if (_temporary_result.is_error()) [[unlikely]]    \
            return _temporary_result.error();             \
        _temporary_result.release_value();                \
    })

#define TRY_F(expression, cleanup)                        \
    ({                                                    \
        auto _temporary_result = (expression);            \
        { cleanup; }                                      \
        if (_temporary_result.is_error()) [[unlikely]]    \
            return _temporary_result.error();             \
        _temporary_result.release_value();                \
    })

#define DARE(expression)                               \
    ({                                                    \
        auto _temporary_result = (expression);            \
        if (_temporary_result.is_error()) [[unlikely]]    \
            core::terminateOnUnhandledError(              \
                _temporary_result.error());               \
        _temporary_result.release_value();                \
    })

#define DARE_F(expression, cleanup)                       \
    ({                                                    \
        auto _temporary_result = (expression);            \
                { cleanup; }                              \
        if (_temporary_result.is_error()) [[unlikely]]    \
            core::terminateOnUnhandledError(              \
                _temporary_result.error());               \
        _temporary_result.release_value();                \
    })

#define TRY_AND_THROW(expression)                         \
    ({                                                    \
        auto _temporary_result = (expression);            \
        if (_temporary_result.is_error()) [[unlikely]] {  \
            core::StringBuilder errorMsg;                 \
            auto& e = _temporary_result.error();          \
            errorMsg.append("core::Error error_code(): ") \
                .append(e.error_code())                   \
                .append(", what(): ")                     \
                .append(e.what());                        \
            throw std::runtime_error(errorMsg);           \
        }                                                 \
        _temporary_result.release_value();                \
    })

#define ON_ERROR(cleanup)                                 \
    ({                                                    \
        if (_temporary_result.is_error()) [[unlikely]]    \
            { cleanup; }                                  \
    })

#ifdef VERIFY
#undef VERIFY
#endif
#define VERIFY(condition, msg) ({                         \
        TRY(core::verify(condition, msg));                \
    })

#endif //__TRY_H__
