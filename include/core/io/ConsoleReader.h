//
// Created by tyll on 2022-01-20.
//

#ifndef SGLOGGER_CONSOLEREADER_H
#define SGLOGGER_CONSOLEREADER_H

#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include "TextReader.h"

namespace core { namespace io {
        class ConsoleReader : public TextReader {
        public:
            int32_t read() override {
                uint8_t c = getch();
                if (c == '\r')
                    c = '\n';
                return (int32_t) c;
            };
        };
    }};

#endif //SGLOGGER_CONSOLEREADER_H
