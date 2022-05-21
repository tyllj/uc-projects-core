//
// Created by tyll on 2022-01-20.
//

#ifndef UC_CORE_TEXTREADER_H
#define UC_CORE_TEXTREADER_H

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
            virtual size_t readLine(char* buffer, int limit) {
                return readToEnd(buffer, '\n', limit);
            };

            /*
             * Reads all characters from the current position to the end of the text reader and returns them as one string.
             */
            virtual size_t readToEnd(char* buffer, int limit) {
                return readToEnd(buffer, '\0', limit);
            };

            virtual size_t readToEnd(char* buffer, char endMark, size_t limit) {
                int32_t r = -1;
                size_t c = 0;
                while (c < limit && (r = read()) != -1 && r != (int32_t)endMark) {
                    buffer[c] = (char) r;
                    c++;
                }
                buffer[c] = '\0';
                return c;
            };

        };
    }};

#endif //UC_CORE_TEXTREADER_H
