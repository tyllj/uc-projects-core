//
// Created by tyll on 2022-01-21.
//

#ifndef SGLOGGER_STREAMWRITER_H
#define SGLOGGER_STREAMWRITER_H

#include "TextWriter.h"
#include "Stream.h"

namespace core { namespace io {
        template<typename StreamImpl>
        class StreamWriter : public TextWriter {
        public:
            explicit StreamWriter(StreamImpl& stream) : _stream(stream) {

            }

            void printf(const char *format, ...) final {

            };

            void write(const unsigned char c) final {
                _stream.writeByte(c);
            }

        private:
            StreamImpl& _stream;
        };
    }};
#endif //SGLOGGER_STREAMWRITER_H
