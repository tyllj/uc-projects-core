//
// Created by tyll on 2022-01-20.
//

#ifndef SGLOGGER_TEXTREADER_H
#define SGLOGGER_TEXTREADER_H

#include <stdint.h>
namespace core { namespace io {
        class TextReader {
        public:
            /*
             * Reads the next character from the text reader and advances the character position by one character.
             */
            virtual int32_t read() = 0;

            /*
             * Reads a line of characters from the text reader and returns the data as a string.
             */
            virtual int32_t readLine(char* buffer, int limit) {
                return readToEnd(buffer, '\n', limit);
            };

            /*
             * Reads all characters from the current position to the end of the text reader and returns them as one string.
             */
            virtual int32_t readToEnd(char* buffer, int limit) {
                return readToEnd(buffer, '\0', limit);
            };

            virtual int32_t readToEnd(char* buffer, char endMark, int32_t limit) {
                int32_t r = -1;
                int32_t c = 0;
                while (c != limit-1 && (r = read()) != -1 && r != (int32_t)endMark) {
                    buffer[c] = (char) r;
                    c++;
                }
                buffer[c] = '\0';
                return c;
            };

        };
    }};

#endif //SGLOGGER_TEXTREADER_H
