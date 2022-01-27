//
// Created by tyll on 2022-01-20.
//

#ifndef SGLOGGER_TEXTWRITER_H
#define SGLOGGER_TEXTWRITER_H

namespace core { namespace io {
    class TextWriter {
    public:

        virtual void write(const char* str) {
            for (const char* c = str; *c != '\0'; c++)
                write(*c);
        }
        virtual void write(const unsigned char c) = 0;

        virtual void writeLine(const char* str) {
            write(str);
            write("\n");
        };
        virtual void printf(const char *format, ...) = 0;
        virtual void flush() {};
    };
}};

#endif //SGLOGGER_TEXTWRITER_H
