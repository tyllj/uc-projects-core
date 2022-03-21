//
// Created by tyll on 2022-01-21.
//

#ifndef SGLOGGER_CONCURRENTQUEUEINGREADERWRITER_H
#define SGLOGGER_CONCURRENTQUEUEINGREADERWRITER_H

#include <stddef.h>
#include "TextReader.h"
#include "TextWriter.h"
#include "QueueStream.h"
#include "core/LockGuard.h"
#include "etl/mutex.h"

namespace core { namespace io {
    template <size_t size>
    class ConcurrentQueueingReaderWriter : public TextWriter, public TextReader {
    public:
        void printf(const char *format, ...) final {

        };

        void write(const unsigned char c) final {
            LockGuard _(_mtx);
            _queue.writeByte(c);
        }

        int32_t read() final {
            LockGuard _(_mtx);
            return _queue.readByte();
        }
    private:
        QueueStream<size> _queue;
        etl::mutex _mtx;
    };
}}
#endif //SGLOGGER_CONCURRENTQUEUEINGREADERWRITER_H
