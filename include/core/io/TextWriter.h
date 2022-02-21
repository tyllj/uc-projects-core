//
// Created by tyll on 2022-01-20.
//

#ifndef SGLOGGER_TEXTWRITER_H
#define SGLOGGER_TEXTWRITER_H

#include "core/CStrings.h"

namespace core { namespace io {

    class TextWriter {
    public:
        void setNewLine(core::cstrings::NewLineMode mode) {
            _newLineMode = mode;
        }

        inline core::cstrings::NewLineMode getNewLine() const {
            return _newLineMode;
        }

        virtual void write(const char* str) {
            for (const char* c = str; *c != '\0'; c++)
                write(*c);
        }
        virtual void write(const unsigned char c) = 0;

        virtual void writeLine(const char* str) {
            write(str);
            write(newLine());
        };
        virtual void printf(const char *format, ...) = 0;
        virtual void flush() {};

    protected:
        inline const char* newLine() const {
            return core::cstrings::newLine(getNewLine());
        }

    private:
        core::cstrings::NewLineMode _newLineMode = core::cstrings::POSIX;


    };
}};

#endif //SGLOGGER_TEXTWRITER_H