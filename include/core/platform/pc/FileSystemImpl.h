//
// Created by tyll on 2022-03-19.
//

#ifndef UC_CORE_FILESYSTEMIMPL_H
#define UC_CORE_FILESYSTEMIMPL_H

#include "etl/result.h"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#include "core/io/FileSystem.h"
#include "core/io/FileSystemInfo.h"
#include "core/io/Stream.h"
#include "core/Error.h"
#include "core/Try.h"

namespace core { namespace platform { namespace pc {
class UnixFileStream : public core::io::Stream {
private:
    typedef etl::unique_ptr<FILE, decltype(&fclose)> NativeFile;
    public:
        static auto open(const char* path, core::io::FileMode mode) -> core::ErrorOr<UnixFileStream> {
            auto file = fopen(path, fileModeToString(mode));
            if (file == nullptr)
                return core::io::FileSystem::Error::CouldNotOpenFile;
            return UnixFileStream(file);
        }

        UnixFileStream(FILE* file) : _file(file, &fclose) {

        }

        void close() final { // TODO: The stream interface should not have a close method, use RAII instead.
            // fclose(_file.get());
        }
        void flush() final {
            fflush(_file.get());
        }
        bool canRead() const final { return true; } // TODO: should reflect the r/w access
        bool canWrite() const final { return true; }// TODO: should reflect the r/w access
        size_t length() const final {
            int32_t pos = position();
            fseek(_file.get(), 0L, SEEK_END);
            int32_t size = position();
            fseek(_file.get(), pos, SEEK_SET);
            return size;
        }
        size_t position() const final { return ftell(_file.get()); }
        void writeByte(uint8_t byte) final {
            fputc(byte, _file.get());
        }
        int32_t seek(int32_t offset) final {
            return fseek(_file.get(), offset, SEEK_CUR);
        }

        int32_t readByte() final {
            uint8_t byte;
            if (((uint8_t) EOF) == (byte = fgetc(_file.get())))
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
        NativeFile _file;
    };

    class NativeFileSystem : public core::io::FileSystem {
    public:
        auto open(const char* path, core::io::FileMode mode) -> core::ErrorOr<etl::unique_ptr<core::io::Stream>> final {
            auto s = TRY(UnixFileStream::open(path, mode));
            return etl::unique_ptr<core::io::Stream>(new UnixFileStream { etl::move(s) });
        };

        auto exists(const char* path) const -> bool final {
            if (access(path, F_OK) == 0)
                return true;
            return false;
        };

        auto forEach(const char* path, etl::delegate<void(core::io::FileSystemInfo&)> action, uint16_t skip = 0, uint16_t take = core::io::FS_TAKEALL, core::io::FileSystemEnumerationOptions options =
        core::io::FILES | core::io::DIRECTORIES) -> uint16_t final {
            uint16_t i = 0;
            uint16_t j = 0;
            DIR *d;
            struct dirent *fsentry;
            struct stat filestat;
            d = opendir(path);
            if (!d)
                return 0; // TODO: return error when path can't be opened.
            while ((fsentry = readdir(d)) != nullptr) {
                if (i >= skip) {
                    if (i - skip < take) {
                        j++;
                        stat(fsentry->d_name, &filestat);
                        auto type = S_ISDIR(filestat.st_mode) ? core::io::FS_DIRECTORY : core::io::FS_FILE;

                        auto fsInfo = core::io::FileSystemInfo(*this, fsentry->d_name, type);
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
            return j;
        };
    };
}}}

#endif //UC_CORE_FILESYSTEMIMPL_H
