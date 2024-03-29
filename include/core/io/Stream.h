//
// Created by tyll on 2022-01-21.
//

#ifndef UC_CORE_STREAM_H
#define UC_CORE_STREAM_H

#include <stddef.h>
#include <stdint.h>

namespace core { namespace io {
    class Stream {
    public:
        virtual ~Stream() {}
        virtual bool canRead() const { return false; }
        virtual bool canWrite() const { return false; }
        virtual size_t length() const { return 0; }
        virtual size_t position() const { return 0; }
        virtual void close() {  }
        virtual void flush() {  }
        virtual int32_t readByte() { return -1; }
        virtual int32_t copyTo(Stream destination) {
            int32_t b;
            int32_t count = 0;
            while ((b = readByte()) != -1) {
                destination.writeByte((uint8_t) b);
                count++;
            }
            return count;
        }

        virtual size_t read(uint8_t* buffer, size_t offset, size_t count) {
            int32_t r = -1;
            size_t c = 0;
            while (c != count && (r = readByte()) != -1) {
                buffer[offset + c] = (uint8_t) r;
                c++;
            }
            return c;
        }

        size_t read(uint8_t* buffer, int32_t count) {
            return read(buffer, 0, count);
        }

        virtual int32_t seek(int32_t offset) {
            int32_t c = 0;
            while (c != offset && readByte() != -1) {
                c++;
            }
            return c;
        }
        virtual void writeByte(const uint8_t) {}
        virtual void write(const uint8_t* buffer, size_t offset, size_t count) {
            for (size_t i = 0; i < count; i++)
                writeByte(buffer[offset + i]);
        }
    };
}}
#endif //UC_CORE_STREAM_H
