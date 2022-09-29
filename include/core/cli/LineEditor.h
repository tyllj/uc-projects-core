//
// Created by tyll on 2022-01-22.
//

#ifndef UC_CORE_LINEEDITOR_H
#define UC_CORE_LINEEDITOR_H

#include <stdio.h>
#include "core/cli/AnsiCharacters.h"
#include "core/io/TextReader.h"
#include "core/io/TextWriter.h"
#include "core/Tick.h"

namespace core { namespace cli {
    template <size_t bufferLength = 80, cstrings::NewLineMode newLineMode = cstrings::NewLineMode::CRLF>
    class LineEditor {
    public:
        LineEditor(
                core::io::TextReader& input,
                core::io::TextWriter& output) :
                _input(input),
                _output(output) {}

        char* readLine() {
            discardLineBuffer();

            do {
                acceptInternal<20>();
            } while (!_isReturn);
            return getBuffer();
        }

        bool accept() {
            return acceptInternal<0>();
        }

        char * getBuffer() {
            return reinterpret_cast<char*>(_lineBuffer);
        }

        void clear() {
            discardLineBuffer();
        }

        void echoOn() {
            _echoToTerminal = true;
        }

        void echoOff() {
            _echoToTerminal = false;
        }

    private:
        template<long delay>
        bool acceptInternal() {
            int32_t c = _input.read();
            if (c > -1) {
                if (_isEscape)
                    handleEscape((uint8_t) c);
                else
                    handleChar((uint8_t) c);
            }
            else if (delay != 0) {
                sleepms(delay);
            }
            return _isReturn;
        }

        void handleEscape(const uint8_t c) {
            if(_isControlSequence) {
                handleControlSequence(c);
                return;
            }
            switch (c) {
                case '[':
                    _isControlSequence = true;
                    break;
                case ESC:
                    _isEscape = false;
                    break;
            }
        }

        void handleControlSequence(const uint8_t c) {
            if (c >= 48 && c <= 57){
                _controlSequenceArgument = 10 * _controlSequenceArgument + (c - 48);
                return;
            }

            switch (c) {
                case 'C':
                    cursorForward();
                    break;
                case 'D':
                    cursorBack();
                    break;
                case '~':
                    handleKeycode(_controlSequenceArgument);
                default:
                    break;
            }

            _isEscape = false;
            _isControlSequence = false;
            _controlSequenceArgument = 0;
        }

        void handleChar(const uint8_t c) {
            switch (c) {
                case NUL:
                case SOH:
                case STX:
                case ETX: // Ctrl+C
                case EOT:
                case ENQ:
                case ACK:
                case BEL:
                case HT:
                case VT:
                case FF:
                case SO:
                case SI:
                case DC1:
                case DC2:
                case DLE:
                case DC3:
                case DC4:
                case NAK:
                case SYN:
                case ETB:
                case CAN:
                case EM:
                case SUB:
                case FS:
                case GS:
                case RS:
                case US:
                    break;
                case ESC:
                    escape();
                    break;
                case LF:
                case CR:
                    carriageReturn();
                    break;
                case DEL:
                case BS:
                    backspace();
                    break;

                default:
                    insert(c);
                    break;
            }
        }

        void handleKeycode(uint8_t keycode) {
            switch (keycode) {
                case 3:
                    deleteFw();
                    break;
                default:
                    break;
            }
        }
        void escape() {
            _isEscape = true;
        }

        void carriageReturn() {
            if (_echoToTerminal && (newLineMode == cstrings::NewLineMode::CR || newLineMode == cstrings::NewLineMode::CRLF))
                terminalPut(CR);
            if (_echoToTerminal && (newLineMode == cstrings::NewLineMode::LF || newLineMode == cstrings::NewLineMode::CRLF))
                terminalPut(LF);
            _isReturn = true;
        }

        void backspace() {
            if (isCursorAtLineStart())
                return;
            for (int i = _cursor; i <= _lineEnd; i++) {
                _lineBuffer[i-1] = _lineBuffer[i];
            }
            _lineEnd--;
            cursorBack();
            terminalControlSequence("0K");
            printBufferFromCursor();
        }

        void deleteFw() {
            if (isCursorAtLineEnd())
                return;
            for (int i = _cursor + 1; i <= _lineEnd; i++) {
                _lineBuffer[i-1] = _lineBuffer[i];
            }
            _lineEnd--;
            terminalControlSequence("0K");
            printBufferFromCursor();
        }

        void insert(const uint8_t c) {
            if (isCursorAtBufferEnd())
                return;

            for (int i = _lineEnd; i > _cursor; i--)
                _lineBuffer[i] = _lineBuffer[i-1];

            _lineEnd++;
            _lineBuffer[_cursor] = c;
            printBufferFromCursor();
            cursorForward();
        }

        void printBufferFromCursor() {
            if (_echoToTerminal) {
                for (int i = _cursor; i < _lineEnd; i++)
                    terminalPut(_lineBuffer[i]);

                terminalCursorBack(_lineEnd - _cursor);
            }
        }
        void cursorBack() {
            if (isCursorAtLineStart())
                return;
            _cursor--;
            terminalCursorBack(1);
        }

        void terminalCursorBack(uint8_t n) {
            for (int i = 0; i < n; i++)
                terminalControlSequence("1D");
        }

        void cursorForward() {
            if (isCursorAtLineEnd())
                return;
            _cursor++;
            terminalCursorForward(1);
        }

        void terminalCursorForward(uint8_t n) {

            for (int i = 0; i < n; i++)
                terminalControlSequence("1C");

        }

        bool isCursorAtLineStart() {
            return _cursor == 0;
        }

        bool isCursorAtLineEnd() {
            return _cursor == _lineEnd;
        }

        bool isCursorAtBufferEnd() {
            return _cursor >= bufferLength - 1;
        }

        void terminalControlSequence(const char* expr) {
            if (_echoToTerminal) {
                terminalPut(ESC);
                terminalPut('[');
                _output.write(expr);
            }
        }

        void terminalPut(const uint8_t c) {
            _output.write(c);
        }

        void discardLineBuffer() {
            for (int32_t i = 0; i < _lineEnd; i++) {
                _lineBuffer[i] = NUL;
            }
            _lineEnd = 0;
            _cursor = 0;
            _isReturn = false;
        }
        
        core::io::TextReader& _input;
        core::io::TextWriter& _output;
        uint8_t _lineBuffer[bufferLength] = { 0 };
        uint8_t _cursor = 0;
        uint8_t _lineEnd = 0;
        bool _isEscape = false;
        bool _isControlSequence = false;
        int32_t _controlSequenceArgument = 0;
        bool _isReturn = false;
        bool _echoToTerminal = true;
    };
}}

#endif //UC_CORE_LINEEDITOR_H
