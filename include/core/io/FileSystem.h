//
// Created by tyll on 2022-03-17.
//

#ifndef UC_CORE_FILESYSTEM_H
#define UC_CORE_FILESYSTEM_H

#include "etl/delegate.h"
#include "core/shared_ptr.h"
#include "Stream.h"

namespace core { namespace io {
    enum FileSystemEnumerationOptions {
        FILES = 1,
        DIRECTORIES = 2
    };

    enum FileMode {
        READ,
        WRITE,
        APPEND,
        READ_UPDATE,
        WRITE_UPDATE,
        APPEND_UPDATE
    };

    constexpr uint16_t FS_TAKEALL = UINT16_MAX;

    inline FileSystemEnumerationOptions operator| (FileSystemEnumerationOptions a, FileSystemEnumerationOptions b) {
        return static_cast<FileSystemEnumerationOptions>(static_cast<int>(a) | static_cast<int>(b));
    };

    class FileSystemInfo; // partial definition needed here to resolve circular dependency.

    class FileSystem {
    public:
        virtual core::shared_ptr<core::io::Stream> open(const char*, FileMode mode) {
            return core::shared_ptr<core::io::Stream>();
        };
        virtual bool exists(const char* path) const {
            return false;
        };
        virtual uint16_t forEach(const char* path, etl::delegate<void(core::io::FileSystemInfo&)> action, uint16_t skip = 0, uint16_t take = FS_TAKEALL, FileSystemEnumerationOptions options =
        FILES | DIRECTORIES) {
            return 0;
        };
    };

    inline FileSystem NullFileSystem;
}}

#endif //UC_CORE_FILESYSTEMIMPL_H
