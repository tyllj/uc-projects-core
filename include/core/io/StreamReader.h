//
// Created by tyll on 2022-01-22.
//

#ifndef SGLOGGER_STREAMREADER_H
#define SGLOGGER_STREAMREADER_H

#include "TextReader.h"

namespace core { namespace io {
        template<typename StreamImpl>
        class StreamReader : public TextReader {
        public:
            StreamReader(StreamImpl& stream) : _stream(stream) {

            }

            int32_t read() final {
                return _stream.readByte();
            }

        private:
            StreamImpl& _stream;
        };
    }};
#endif //SGLOGGER_STREAMREADER_H
