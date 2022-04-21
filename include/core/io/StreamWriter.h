//
// Created by tyll on 2022-01-21.
//

#ifndef UC_CORE_STREAMWRITER_H
#define UC_CORE_STREAMWRITER_H

#include "TextWriter.h"
#include "Stream.h"

namespace core { namespace io {
        template<typename StreamImpl>
        class StreamWriter : public TextWriter {
        public:
            explicit StreamWriter(StreamImpl& stream) : _stream(stream) {

            }

            void write(const char* str) final {
                this->TextWriter::write(str);
            }

            void write(const unsigned char c) final {
                _stream.writeByte(c);
            }

        private:
            StreamImpl& _stream;
        };
    }};
#endif //UC_CORE_STREAMWRITER_H
