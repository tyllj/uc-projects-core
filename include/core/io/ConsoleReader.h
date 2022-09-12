//
// Created by tyll on 2022-01-20.
//

#ifndef UC_CORE_CONSOLEREADER_H
#define UC_CORE_CONSOLEREADER_H

#include <stdio.h>
#include <stdint.h>


#if __has_include (<conio.h>)
#include <conio.h>
#else
#include "conio_nc.h"
#endif

#include "TextReader.h"

namespace core { namespace io {
        class ConsoleReader : public TextReader {
        public:
            int32_t read() final {
                if (!kbhit())
                    return -1;
                uint8_t c = getch();
                if (c == '\r')
                    c = '\n';
                return (int32_t) c;
            };
        };
    }};

#endif //UC_CORE_CONSOLEREADER_H
