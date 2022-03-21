//
// Created by tyll on 2022-01-26.
//

#ifndef SGLOGGER_FILESYSTEMINFO_H
#define SGLOGGER_FILESYSTEMINFO_H

#include <string.h>
#include "Stream.h"
#include "TextWriter.h"
#include "core/shared_ptr.h"
#include "core/CStrings.h"
#include "core/StringBuilder.h"
#include "FileSystem.h"
#include "Path.h"

namespace core { namespace io {
    enum FileSystemEntryType {
        FS_FILE = 0,
        FS_DIRECTORY = 1
    };

    // Base class for file system navigation
    class FileSystemInfo {
    public:

        FileSystemInfo(FileSystem& fileSystem, const char* path, const FileSystemEntryType type) :
            _fullName(new char[strlen(path)+1]),
            _fileSystem(&fileSystem),
            _type(type) {
            strcpy(_fullName.get(), path);
        }

        const FileSystemEntryType getType() const {
            return _type;
        }

        const char* getName() const {
            return (const char*) _fullName.get();
        }

        bool exists() const {
            return getFileSystem().exists(getFullName());
        }
        const char* getFullName() const {
            return (const char*) _fullName.get();
        }

    protected:
        FileSystem& getFileSystem() const {
            return *_fileSystem;
        }

    private:
        FileSystem* _fileSystem;
        shared_ptr<char[]> _fullName;
        FileSystemEntryType _type;

        friend class DirectoryInfo;
        friend class FileInfo;
    };

    class DirectoryInfo : public FileSystemInfo {
    public:
        DirectoryInfo(FileSystem& fileSystem, const char* path) :
            FileSystemInfo(fileSystem, path, FS_FILE) {
        }

        DirectoryInfo(DirectoryInfo& directoryInfo, const char* path) :
            FileSystemInfo(directoryInfo.getFileSystem(), path, FS_FILE) {
        }

        DirectoryInfo getDirectory() {
            core::StringBuilder sb;
            core::io::path::getDirectoryName(sb, getFullName());
            return DirectoryInfo(*this, sb.toString());
        }

        uint16_t forEach(etl::delegate<void(FileSystemInfo&)> action) {
            return getFileSystem().forEach(getFullName(), action);
        }

        etl::optional<DirectoryInfo> createSubDirectory() {
            return etl::optional<DirectoryInfo>();
        };
    };

    class FileInfo : public FileSystemInfo {
    public:
        explicit FileInfo(FileSystem& fileSystem, const char* path) :
            FileSystemInfo(fileSystem, path, FS_FILE) {
        }

        explicit FileInfo(DirectoryInfo& fileSystemInfo, const char* path) :
            FileSystemInfo(fileSystemInfo.getFileSystem(), path, FS_FILE) {

        }

        DirectoryInfo getDirectory() {
            return DirectoryInfo(this->FileSystemInfo::getFileSystem(), getFullName());
        }

        core::shared_ptr<Stream> open(core::io::FileMode mode) {
            return getFileSystem().open(getFullName(), mode);
        };
    };
}}
#endif //SGLOGGER_FILESYSTEMINFO_H
