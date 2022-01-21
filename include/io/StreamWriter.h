//
// Created by tyll on 2022-01-21.
//

#ifndef SGLOGGER_STREAMWRITER_H
#define SGLOGGER_STREAMWRITER_H

#include "AbstractTextWriter.h"

namespace core { namespace io {
        class StreamWriter : public AbstractTextWriter {
        public:
            virtual void printf(const char *format, ...) override;
            virtual void write(const char* str) override;
            virtual void writeLine(const char* str) override;
            virtual void flush() override;

        private:

        };
    }};
#endif //SGLOGGER_STREAMWRITER_H
