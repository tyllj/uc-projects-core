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

namespace core { namespace platform { namespace pc {
class UnixFileStream : public core::io::Stream {
    public:
        UnixFileStream(const char* path, core::io::FileMode mode) {
            _file = fopen(path, fileModeToString(mode));
        }
        ~UnixFileStream() {
            flush();
            close();
        }
        void close() final {
            if (_file != NULL)
                fclose(_file);
        }
        void flush() final {
            if (_file != NULL)
                fflush(_file);
        }
        bool canRead() const final { return _file != NULL; }
        bool canWrite() const final { return _file != NULL; }
        size_t length() const final {
            if (_file == NULL)
                return 0;
            int32_t pos = position();
            fseek(_file, 0L, SEEK_END);
            int32_t size = position();
            fseek(_file, pos, SEEK_SET);
            return size;
        }
        size_t position() const final { return ftell(_file); }
        void writeByte(uint8_t byte) final {
            if (_file != NULL)
                fputc(byte, _file);
        }
        int32_t seek(int32_t offset) final {
            if (_file == NULL)
                return 0;
            return fseek(_file, offset, SEEK_CUR);
        }

        int32_t readByte() final {
            uint8_t byte;
            if (_file == NULL || ((uint8_t) EOF) == (byte = fgetc(_file)))
                return -1;
            return (int32_t) byte;
        }

    private:
        static const char* fileModeToString(core::io::FileMode mode) {
            switch (mode) {
                case core::io::READ: return "rb";
                case core::io::WRITE: return "wb";
                case core::io::APPEND: return "ab";
                case core::io::READ_UPDATE: return "r+b";
                case core::io::WRITE_UPDATE: return "w+b";
                case core::io::APPEND_UPDATE: return "a+b";
                default: return "";
            }
        }

    private:
        FILE* _file;
    };

    class NativeFileSystem : public core::io::FileSystem {
    public:
        core::shared_ptr<core::io::Stream> open(const char* path, core::io::FileMode mode) final {
            return core::shared_ptr<core::io::Stream>(new UnixFileStream(path, mode));
        };

        bool exists(const char* path) const final {
            if (access(path, F_OK) == 0)
                return true;
            return false;
        };

        uint16_t forEach(const char* path, etl::delegate<void(core::io::FileSystemInfo&)> action, uint16_t skip = 0, uint16_t take = core::io::FS_TAKEALL, core::io::FileSystemEnumerationOptions options =
        core::io::FILES | core::io::DIRECTORIES) final {
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
                            stat(fsentry->d_name, &filestat);
                            core::io::FileSystemEntryType type;
                            type = S_ISDIR(filestat.st_mode) ? core::io::FS_DIRECTORY : core::io::FS_FILE;

                            core::io::FileSystemInfo fsInfo(*this, fsentry->d_name, type);
                            if (((options | core::io::FILES) && (type == core::io::FS_FILE)) ||
                                    ((options | core::io::DIRECTORIES) && (type == core::io::FS_DIRECTORY)))
                                action(fsInfo);
                        }
                        else
                            break;
                    }
                    i++;
                }
                closedir(d);
            }
            return j;
        };
    };
}}}

#endif //UC_CORE_FILESYSTEMIMPL_H
