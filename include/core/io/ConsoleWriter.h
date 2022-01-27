//
// Created by tyll on 2022-01-20.
//

#ifndef SGLOGGER_CONSOLEWRITER_CPP
#define SGLOGGER_CONSOLEWRITER_H

#include <stdio.h>
#include <stdarg.h>
#include "TextWriter.h"

namespace core { namespace io {
        class ConsoleWriter : public TextWriter {
        public:
            virtual void printf(const char *format, ...) {
                va_list ap;

                va_start(ap, format);
                int result = vprintf(format, ap);
                va_end(ap);
            };

            virtual void write(const unsigned char c) {
                putchar(c);
            }
            virtual void write(const char* bytes, uint32_t count) {
                for (uint32_t i = 0; i < count; i++)
                    if (putchar(bytes[i]) == EOF)
                        return;
            }
            virtual void flush() {}
        };
    }};

#endif //SGLOGGER_CONSOLEWRITER_CPP
