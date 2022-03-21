//
// Created by tyll on 2022-03-17.
//

#ifndef SGLOGGER_FILESYSTEM_H
#define SGLOGGER_FILESYSTEM_H

#include "Stream.h"
#include "core/shared_ptr.h"
#include "FileSystemInfo.h"
#include "FileSystem.h"

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
        virtual core::shared_ptr<Stream> open(const char* path, FileMode mode) {
            return {};
        };
        virtual bool exists(const char* path) {
            return false;
        };
        virtual uint16_t forEach(const char* path, etl::delegate<void(core::io::FileSystemInfo&)> action, uint16_t skip = 0, uint16_t take = FS_TAKEALL, FileSystemEnumerationOptions options =
        FILES | DIRECTORIES) {
            return 0;
        };
    };

    static FileSystem NullFileSystem;
}}

#endif //SGLOGGER_FILESYSTEMIMPL_H
