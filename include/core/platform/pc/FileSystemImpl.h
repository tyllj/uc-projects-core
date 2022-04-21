//
// Created by tyll on 2022-03-19.
//

#ifndef UC_CORE_FILESYSTEMIMPL_H
#define UC_CORE_FILESYSTEMIMPL_H


#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#include "core/io/FileSystem.h"
#include "core/io/FileSystemInfo.h"
#include "core/io/Stream.h"

namespace core { namespace io {
    class NativeFileStream : public Stream {
    public:
        NativeFileStream(const char* path, core::io::FileMode mode) {
            _file = fopen(path, fileModeToString(mode));
        }
        ~NativeFileStream() {
            flush();
            close();
        }
        virtual void close() {
            if (_file != NULL)
                fclose(_file);
        }
        virtual void flush() {
            if (_file != NULL)
                fflush(_file);
        }
        virtual bool canRead() const { return _file != NULL; }
        virtual bool canWrite() const { return _file != NULL; }
        virtual int32_t getLength() const {
            if (_file == NULL)
                return 0;
            int32_t pos = getPosition();
            fseek(_file, 0L, SEEK_END);
            int32_t size = getPosition();
            fseek(_file, pos, SEEK_SET);
            return size;
        }
        virtual int32_t getPosition() const { return ftell(_file); }
        void writeByte(uint8_t byte) override {
            if (_file != NULL)
                fputc(byte, _file);
        }
        virtual int32_t seek(int32_t offset) override {
            if (_file == NULL)
                return 0;
            return fseek(_file, offset, SEEK_CUR);
        }

        int32_t readByte() override {
            uint8_t byte;
            if (_file == NULL || ((uint8_t) EOF) == (byte = fgetc(_file)))
                return -1;
            return (int32_t) byte;
        }

    private:
        static const char* fileModeToString(core::io::FileMode mode) {
            switch (mode) {
                case READ: return "rb";
                case WRITE: return "wb";
                case APPEND: return "ab";
                case READ_UPDATE: return "r+b";
                case WRITE_UPDATE: return "w+b";
                case APPEND_UPDATE: return "a+b";
                default: return "";
            }
        }

    private:
        FILE* _file;
    };

    class NativeFileSystem : public FileSystem {
    public:
        virtual core::shared_ptr<Stream> open(const char* path, core::io::FileMode mode) override {
            return core::shared_ptr<Stream>(new NativeFileStream(path, mode));
        };
        virtual bool exists(const char* path) override {
            if (access(path, F_OK) == 0)
                return true;
            return false;
        };
        virtual uint16_t forEach(const char* path, etl::delegate<void(core::io::FileSystemInfo&)> action, uint16_t skip = 0, uint16_t take = FS_TAKEALL, FileSystemEnumerationOptions options =
        FILES | DIRECTORIES) {
            uint16_t i = 0;
            uint16_t j = 0;
            DIR *d;
            struct dirent *fsentry;
            struct stat filestat;
            d = opendir(path);
            if (d) {
                while ((fsentry = readdir(d)) != NULL) {
                    if (i >= skip) {
                        if (i - skip < take) {
                            j++;
                            stat(fsentry->d_name,&filestat);
                            FileSystemEntryType type;
                            type = S_ISDIR(filestat.st_mode) ? FS_DIRECTORY : FS_FILE;

                            core::io::FileSystemInfo fsInfo(*this, fsentry->d_name, type);
                            if (((options | FILES) && (type == FS_FILE)) ||
                                    ((options | DIRECTORIES) && (type == FS_DIRECTORY)))
                                action(fsInfo);
                        }
                        else
                            break;
                    }
                }
                closedir(d);
            }
            return j;
        };
    };
}}

#endif //UC_CORE_FILESYSTEMIMPL_H
