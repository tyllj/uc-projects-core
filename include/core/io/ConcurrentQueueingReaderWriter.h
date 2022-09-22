//
// Created by tyll on 2022-01-21.
//

#ifndef UC_CORE_CONCURRENTQUEUEINGREADERWRITER_H
#define UC_CORE_CONCURRENTQUEUEINGREADERWRITER_H

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

        void write(char c) final {
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
#endif //UC_CORE_CONCURRENTQUEUEINGREADERWRITER_H
