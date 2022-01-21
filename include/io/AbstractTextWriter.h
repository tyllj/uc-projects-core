//
// Created by tyll on 2022-01-20.
//

#ifndef SGLOGGER_ABSTRACTTEXTWRITER_H
#define SGLOGGER_ABSTRACTTEXTWRITER_H

namespace core { namespace io {
    class AbstractTextWriter {
    public:
        virtual void write(const char c) = 0;
        virtual void write(const char* str) {
            for (const char* c = str; c != NULL; c++)
                write(*c);
        }
        virtual void writeLine(const char* str) {
            write(str);
            write("\n");
        };
        virtual void printf(const char *format, ...) = 0;
        virtual void flush() {};
    };
}};

#endif //SGLOGGER_ABSTRACTTEXTWRITER_H
