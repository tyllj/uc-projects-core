//
// Created by tyll on 2022-01-21.
//

#ifndef SGLOGGER_STREAM_H
#define SGLOGGER_STREAM_H

#include <stdint.h>

namespace core { namespace io {
    class Stream {
    public:
        virtual bool canRead() { return false; }
        virtual bool canWrite() { return false; }
        virtual bool canTimeout() { return false; }
        virtual int32_t getLength() { return 0; }
        virtual int32_t getPosition() { return 0; }
        virtual void close() {  }
        virtual void flush() {  }
        virtual int32_t readByte() { return -1; }
        virtual int32_t read(uint8_t* buffer, int32_t offset, int32_t count) {
            int32_t r = -1;
            int32_t c = 0;
            while (c != count && (r = readByte()) != -1) {
                buffer[offset + c] = (uint8_t) r;
                c++;
            }
            return c;
        }
        virtual int32_t read(uint8_t* buffer, int32_t count) {
            return read(buffer, 0, count);
        }

        virtual int32_t seekBegin(int32_t offset);
        virtual int32_t seek(int32_t offset) {
            int32_t c = 0;
            while (c != offset && readByte() != -1) {
                c++;
            }
            return c;
        }
        virtual void writeByte(uint8_t byte) {}
        virtual void write(uint8_t* buffer, int32_t offset, int32_t count) {
            for (int32_t i = 0; i < count; i++)
                writeByte(buffer[offset + i]);
        }
    };
}}
#endif //SGLOGGER_STREAM_H
