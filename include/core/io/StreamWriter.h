//
// Created by tyll on 2022-01-21.
//

#ifndef UC_CORE_STREAMWRITER_H
#define UC_CORE_STREAMWRITER_H

#include "TextWriter.h"
#include "Stream.h"
#include "core/CStrings.h"

namespace core { namespace io {
        class StreamWriter : public TextWriter {
        public:
            explicit StreamWriter(Stream& stream) : _stream(stream) {

            }

            void write(const char* str) final {
                _stream.write(reinterpret_cast<const uint8_t *>(str), 0, cstrings::length(str));
            }

            void write(char c) final {
                _stream.writeByte(c);
            }

        private:
            Stream& _stream;
        };
    }};
#endif //UC_CORE_STREAMWRITER_H
