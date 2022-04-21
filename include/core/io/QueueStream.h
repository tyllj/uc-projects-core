//
// Created by tyll on 2022-01-21.
//

#ifndef UC_CORE_QUEUESTREAM_H
#define UC_CORE_QUEUESTREAM_H

#include "Stream.h"

#define CIRCULAR_BUFFER_INT_SAFE
#include "CircularBuffer/CircularBuffer.h"

namespace core { namespace io {
    template <size_t size>
    class QueueStream : public Stream {
    public:
        bool canRead() const override { return true; }
        bool canWrite() const override { return true; }
        int32_t getLength() const override { return size; }
        int32_t getPosition() const override { return 0; }
        void close() override {  }
        void flush() override {  }
        int32_t readByte() override {
            if (_buffer.isEmpty())
                return -1;
            return (int32_t) _buffer.shift();
        }
        void writeByte(uint8_t byte) override {
            if (!_buffer.isFull())
               _buffer.push(byte);
        }
    private:
        CircularBuffer<uint8_t, size> _buffer;
    };
}}

#endif //UC_CORE_QUEUESTREAM_H
