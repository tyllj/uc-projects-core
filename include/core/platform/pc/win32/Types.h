//
// Created by tu.jungke on 20.12.2022.
//

#ifndef __TYPES_H__
#define __TYPES_H__

#include <memory>
#include <cstring>
#include <windows.h>
#include "core/Error.h"
#include "core/CStrings.h"
#include "core/StringBuilder.h"

namespace core { namespace platform{ namespace pc {
    using Win32DynamicLibrary = std::unique_ptr<std::remove_pointer_t<HMODULE>, decltype(&FreeLibrary)>;
    using Win32Handle = std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(&CloseHandle)>;

    inline auto loadLibrary(const char* libraryPath) -> ErrorOr<Win32DynamicLibrary> {
        auto nativeHandle = LoadLibraryA(libraryPath);
        if (nativeHandle == nullptr) {
            static char errorMsg[128];
            core::StringBuilder(errorMsg).append("Failed to load library: ").append(libraryPath);
            return core::Error(0x00BADD11, errorMsg);
        }

        return Win32DynamicLibrary(nativeHandle, &FreeLibrary);
    }

    inline auto toSafeHandle(HANDLE nativeHandle) -> ErrorOr<Win32Handle> {
        if (nativeHandle == nullptr || nativeHandle == INVALID_HANDLE_VALUE)
            return core::Error(0xBADACCE5, "Handle was NULL or invalid.");
        return Win32Handle(nativeHandle, &CloseHandle);
    }
}}}
#endif //__TYPES_H__
