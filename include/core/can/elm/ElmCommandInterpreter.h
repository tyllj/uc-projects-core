//
// Created by tyll on 2022-09-11.
//

#ifndef SGLOGGER_ELMCOMMANDINTERPRETER_H
#define SGLOGGER_ELMCOMMANDINTERPRETER_H

#include "ElmRuntimeConfiguration.h"
#include "core/cli/LineEditor.h"
#include "core/io/Stream.h"
#include "core/io/StreamWriter.h"
#include "core/Convert.h"
#include "core/shared_ptr.h"
#include "core/async/Future.h"

namespace core { namespace can { namespace elm {
    class ElmCommandInterpreter {
    private:
        enum State {
            READY,
            BUSY
        };

    public:
        ElmCommandInterpreter(core::async::IDispatcher& dispatcher, core::io::Stream& tty, ICanInterface &can)
                :
                _dispatcher(dispatcher),
                _in(tty),
                _out(tty),
                _prompt(_in, _out),
                _can(can) {
            _out.setNewLine(core::cstrings::NewLineMode::CR);
            startBackgroundWorker(_dispatcher);
        }

        ~ElmCommandInterpreter() {
            if (_backgroundWorkerTask)
                _backgroundWorkerTask->cancel();
        }

    private:
        void startBackgroundWorker(async::IDispatcher &dispatcher) {
            beginAcceptInput();
            _backgroundWorkerTask = FUTURE_FROM_MEMBER(ElmCommandInterpreter, process);
            dispatcher.run(_backgroundWorkerTask);
        }

        void process() {
            switch (_state) {
                case READY:
                    continueAcceptInput();
                    break;
                case BUSY:
                    continueReceive();
                    break;
            }
        }

        void beginAcceptInput() {
            _state = READY;
            _out.write("\r>");
            _prompt.clear();
        }

        void continueAcceptInput() {
            if (_prompt.accept()) {
                dispatchCommand(_prompt.getBuffer());
            }
        }

        void beginReceive() {
            // setup iso-tp socket
            _state = BUSY;
        }

        void continueReceive() {
            if (_in.read() != -1) {
                endReceive();
                return;
            }
        }

        void endReceive() {
            // teardown iso-tp socket, clear queues.

            beginAcceptInput();
        }

        void printVersion(const char* param) {
            respond(ELM_VERSION_STRING);
        }

        void warmStart(const char* param) {
            respond(ELM_VERSION_STRING);
        }

        void resetAll(const char* param) {
            respond(ELM_VERSION_STRING);
        }

        void printProgramableParameters(const char* param) {
            respond(ELM_PROGRAMMABLE_PARAMETERS);
        }

        void echoOn(const char* param) {
            _config.isEchoOn = true;
            _prompt.echoOn();
            respondOk();
        }

        void echoOff(const char* param) {
            _config.isEchoOn = false;
            _prompt.echoOff();
            respondOk();
        }

        void lineFeedsOn(const char* param) {
            _config.isLineFeedsOn = true;
            respondOk();
        }

        void lineFeedsOff(const char* param) {
            _config.isLineFeedsOn = false;
            respondOk();
        }

        void headersOn(const char* param) {
            _config.isHeadersOn = true;
            respondOk();
        }

        void headersOff(const char* param) {
            _config.isHeadersOn = false;
            respondOk();
        }

        void responsesOn(const char* param) {
            _config.isResponsesOn = true;
            respondOk();
        }

        void responsesOff(const char* param) {
            _config.isResponsesOn = false;
            respondOk();
        }

        void setTimeout(const char* param) {
            _config.timeoutMultiplier = core::convert::toUInt8(param, 16);
            respondOk();
        }

        void setTesterAddress(const char* param) {
            _config.testerAddress = core::convert::toUInt8(param, 16);
            respondOk();
        }

        void printSpacesOn(const char* param) {
            _config.isSpacesOn = true;
            respondOk();
        }

        void printSpacesOff(const char* param) {
            _config.isSpacesOn = false;
            respondOk();
        }

        void setAdaptiveTimingMode(const char* param) {
            _config.adaptiveTimingMode = core::convert::toUInt8(param);
            respondOk();
        }

        void setFilter(const char* param) {
            _config.filterId = core::convert::toUInt16(param, 16);
            _can.filter(_config.filterId);
            respondOk();
        }

        void setMask(const char* param) {
            _config.maskId = core::convert::toUInt16(param, 16);
            _can.mask(_config.maskId);
            respondOk();
        }

        void readInputVoltage(const char* param) {
            respond("12.5V");
        }

        void autoFormattingOn(const char* param) {
            _config.isAutoFormattingOn = true;
            respondOk();
        }

        void autoFormattingOff(const char* param) {
            _config.isAutoFormattingOn = false;
            respondOk();
        }

        void tryProtocol(const char* param) {
            // Assert protocol is 6 (CAN, ISO 15765-4, 11 bit ID, 500 kbaud), because other protocols are not supported atm.
            if (core::cstrings::equals(param, "6")
                    || core::cstrings::equals(param, "0"))
                respondOk();
            else
                respond("BUS INIT: BUS ERROR");
        }

        void allowLongMessages(const char* param) {
            respondOk();
        }

        void setHeader(const char* param) {
            _config.transmitAddress = core::convert::toUInt16(param, 16);
        }

        void setCanReceiveAddress(const char* param) {
            _config.filterId = core::convert::toUInt16(param, 16);
            _can.filter(_config.filterId);
            _config.maskId = CANID_11_MAX;
            _can.mask(_config.maskId);
        }

        void protocolClose(const char* param) {
            respondOk();
        }

        bool isCommand(const char* input, const char* code, const char** outParameter) {
            if (cstrings::startsWith(input, code)) {
                *outParameter = input + cstrings::length(code);
                return true;
            }

            return false;
        }

        void dispatchCommand(const char* input) {
            if (cstrings::isNullOrWhitespace(input)) {
                if (cstrings::isNullOrEmpty(_command))
                    respondUnkown();
                else
                    dispatchCommand(_command);
                return;
            }

            const char* param;

            cstrings::copy(_command, input);
            cstrings::trim(_command);
            cstrings::toUpper(_command);

            if (!cstrings::startsWith(_command, "AT")) {
                // handle as raw data, send to can or isotp
                return;
            }

            if (isCommand(_command, "ATRV", &param))
                readInputVoltage(param);
            else if (isCommand(_command, "ATSH", &param))
                setHeader(param);
            else if (isCommand(_command, "ATCRA", &param))
                setCanReceiveAddress(param);
            else if (isCommand(_command, "ATCAF0", &param))
                autoFormattingOff(param);
            else if (isCommand(_command, "ATCAF1", &param))
                autoFormattingOn(param);
            else if (isCommand(_command, "ATST", &param))
                setTimeout(param);
            else if (isCommand(_command, "ATR0", &param))
                responsesOff(param);
            else if (isCommand(_command, "ATR1", &param))
                responsesOn(param);
            else if (isCommand(_command, "ATCF", &param))
                setFilter(param);
            else if (isCommand(_command, "ATCM", &param))
                setMask(param);
            else if (isCommand(_command, "ATTA", &param))
                setTesterAddress(param);
            else if (isCommand(_command, "ATPC", &param))
                protocolClose(param);
            else if (isCommand(_command, "ATE0", &param))
                echoOff(param);
            else if (isCommand(_command, "ATE1", &param))
                echoOn(param);
            else if (isCommand(_command, "ATL0", &param))
                lineFeedsOff(param);
            else if (isCommand(_command, "ATL1", &param))
                lineFeedsOn(param);
            else if (isCommand(_command, "ATH0", &param))
                headersOff(param);
            else if (isCommand(_command, "ATH1", &param))
                headersOn(param);
            else if (isCommand(_command, "ATS0", &param))
                printSpacesOff(param);
            else if (isCommand(_command, "ATS1", &param))
                printSpacesOn(param);
            else if (isCommand(_command, "ATTP", &param))
                tryProtocol(param);
            else if (isCommand(_command, "ATAL", &param))
                allowLongMessages(param);
            else if (isCommand(_command, "ATPPS", &param))
                printProgramableParameters(param);
            else if (isCommand(_command, "ATI", &param))
                printVersion(param);
            else if (isCommand(_command, "ATWS", &param))
                warmStart(param);
            else if (isCommand(_command, "ATZ", &param))
                resetAll(param);
            else
                respondUnkown();
        }

        void respondUnkown() {
            respond("?");
        }

        void respondOk() {
            respond("OK");
        }

        void respond(const char* response) {
            _out.writeLine(response);
        }

    private:
        core::async::IDispatcher& _dispatcher;
        ElmRuntimeConfiguration _config;
        core::io::StreamReader _in;
        core::io::StreamWriter _out;
        core::cli::LineEditor<40, cstrings::CR> _prompt;
        char _command[40];
        core::can::ICanInterface& _can;
        etl::optional<core::can::IsoTpSocket> _isotp;
        core::shared_ptr<async::IFuture> _backgroundWorkerTask;
        State _state;

    };
}}}

#endif //SGLOGGER_ELMCOMMANDINTERPRETER_H
