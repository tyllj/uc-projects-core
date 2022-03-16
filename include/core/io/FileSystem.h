//
// Created by tyll on 2022-01-26.
//

#ifndef SGLOGGER_FILESYSTEM_H
#define SGLOGGER_FILESYSTEM_H

#include "Stream.h"
#include "DirectoryInfo.h"
#include "core/io/TextWriter.h"
#include "core/shared_ptr.h"

namespace core { namespace io {
    // Base class for file systems
    class FileSystemInfo {
    public:
        const char* getName() const {
            return _name;
        }

        virtual bool exists() = 0;
        virtual uint8_t getFullName(char * destStr, uint8_t limit) {
            return 0;
        }

    private:
        const char* _name;
    };

    class DirectoryInfo {
        shared_ptr<DirectoryInfo> getDirectory() {
            return _directory;
        }

        shared_ptr<DirectoryInfo> createSubDirectory() {

        }

    private:
        shared_ptr<DirectoryInfo> _directory;
    };

    class FileInfo : public FileSystemInfo {
    public:
        shared_ptr<DirectoryInfo> getDirectory() {
            return _directory;
        }

        virtual uint64_t getLength() = 0;

        virtual core::io::Stream create() = 0;
        virtual core::io::Stream open() = 0;

    private:
        shared_ptr<DirectoryInfo> _directory;
    };


}}
#endif //SGLOGGER_FILESYSTEM_H
