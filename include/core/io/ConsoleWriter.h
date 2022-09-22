//
// Created by tyll on 2022-01-20.
//

#ifndef UC_CORE_CONSOLEWRITER_CPP
#define UC_CORE_CONSOLEWRITER_H

#include <stdio.h>
#include <stdarg.h>
#include "TextWriter.h"

namespace core { namespace io {
        class ConsoleWriter : public TextWriter {
        public:
            void write(char c) final {
                putchar(c);
            }
            virtual void write(const  char* str) final {
                puts(str);
            }
            virtual void flush() {}
        };
    }};

#endif //UC_CORE_CONSOLEWRITER_CPP
