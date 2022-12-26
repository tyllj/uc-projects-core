//
// Created by tyll on 2022-09-11.
//

#ifndef UC_CORE_ELMCOMMANDINTERPRETER_H
#define UC_CORE_ELMCOMMANDINTERPRETER_H

#include "ElmRuntimeConfiguration.h"
#include "core/StringBuilder.h"
#include "core/Tick.h"
#include "core/Convert.h"
#include "core/shared_ptr.h"
#include "core/Future.h"
#include "core/cli/AnsiCharacters.h"
#include "core/io/Stream.h"
#include "core/io/StreamReader.h"
#include "core/io/StreamWriter.h"
#include "core/can/IsoTpSocket.h"



namespace core { namespace can { namespace elm {
    class ElmCommandInterpreter {
    private:
        enum State {
            READY,
            BUSY
        };

    public:
        ElmCommandInterpreter(core::IDispatcher& dispatcher, core::io::Stream& tty, ICanInterface &can)
                :
                _dispatcher(dispatcher),
                _in(tty),
                _out(tty),
                _prompt(_input),
                _can(can) {
            _out.setNewLine(core::cstrings::NewLineMode::CR);
            while(tty.readByte() != -1); // flush receive buffer
            startBackgroundWorker(_dispatcher);
        }

        ~ElmCommandInterpreter() {
            if (_backgroundWorkerTask)
                _backgroundWorkerTask->cancel();
        }

    private:
        void startBackgroundWorker(IDispatcher &dispatcher) {
            beginAcceptInput();
            _backgroundWorkerTask = async([this]() {
                this->process();
                return yieldContinue();
            }).runOn(dispatcher);
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
            char c;
            if ((c = _in.read()) != -1) {
                _prompt.append(c);
                if (_config.isEchoOn)
                    _out.write(c);
            }
            if (c == CR) {
                dispatchCommand(_prompt);
                _prompt.clear();
                if (_state == READY)
                    beginAcceptInput();
            }
        }

        uint64_t getTimeout() {
            return _config.timeoutMultiplier * 4;
        }

        void beginReceive() {
            // setup iso-tp socket
            _bytesReceived = false;
            _startedReceiving = millis();
            _state = BUSY;
        }

        void continueReceive() {
            if (_in.read() != -1) {
                _out.writeLine("STOPPED");
                endReceive();
                return;
            }
            if (millis() - _startedReceiving > getTimeout()) {
                if (!_bytesReceived)
                    _out.writeLine("NO DATA");
                endReceive();
                return;
            }

            if (_config.isAutoFormattingOn) {
                IsoTpPacket p;
                if (_isotp->tryReceive(p)) {
                    printMessageFormatted(p.canid(), p.getData(), p.length());
                    endReceive();
                }
            } else {
                CanFrame f;
                if (_can.tryReadFrame(f)) {
                    printMessageRaw(f.id, f.payload, f.length);
                    _bytesReceived = true;
                }
            }
        }

        void printMessageRaw(canid_t sender, uint8_t* bytes, size_t length) {
            char byteString[3] = { 0x00 };
            if (_config.isHeadersOn) {
                // print sender 
            }
            for (uint8_t i = 0; i < length; i++) {
                convert::toHexString(bytes++, 1, byteString);
                cstrings::toUpper(byteString);
                _out.write(byteString);
                if (_config.isSpacesOn && i < length + 1)
                    _out.write(' ');
            }
            _out.writeLine();
        }

        void printMessageFormatted(canid_t sender, uint8_t* bytes, size_t length) {
            if (length < 7) { // single line response, treat as raw
                printMessageRaw(sender, bytes, length);
                return;
            }


            if (_config.isHeadersOn) {
                // Not supported now
            } else {

            }
        }

        void endReceive() {
            // teardown iso-tp socket, clear queues.
            if (_isotp.has_value())
                deinitIsoTp();
            beginAcceptInput();
        }

        void sendToVehicle(const char* hexData) {
            switch (_config.protocol) {
                case 1:
                case 2:
                    respond("FB ERROR");
                    return;
                case 3:
                case 4:
                case 5:
                    respond(_config.bypassInit ? "BUS ERROR" : "BUS INIT: BUS ERROR");
                    return;
                case 6:
                    break;
                default:
                    respond("CAN ERROR");
                    return;
            }


            uint8_t message[32] = { 0x00 };
            size_t dlc = convert::fromHexString(hexData, message);

            if(_config.isAutoFormattingOn) {
                initIsoTp();
                _isotp->send(message, dlc);
            } else {
                CanFrame f;
                f.id = _config.transmitAddress;
                f.length = 8;
                memcpy(f.payload, message, f.length);
                _can.writeFrame(f);
            }

            beginReceive();
        }

        void printVersion(const char* param) {
            respond(ELM_VERSION_STRING);
        }

        void warmStart(const char* param) {
            _config = ElmRuntimeConfiguration();
            respond(ELM_VERSION_STRING);
        }

        void resetAll(const char* param) {
            _config = ElmRuntimeConfiguration();
            respond(ELM_VERSION_STRING);
        }

        void printProgramableParameters(const char* param) {
            respond(ELM_PROGRAMMABLE_PARAMETERS);
        }

        void echoOn(const char* param) {
            _config.isEchoOn = true;
            respondOk();
        }

        void echoOff(const char* param) {
            _config.isEchoOn = false;
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

            if (cstrings::length(param) != 1 ||
                    !convert::isHexString(param)) {
                respondUnkown();
                return;
            }

            _config.protocol = convert::toUInt8(param, 16);
            respondOk();
        }

        void bypassInit(const char* param) {
            _config.bypassInit = true;
            respondOk();
        }

        void allowLongMessages(const char* param) {
            respondOk();
        }

        void normalLengthMessages(const char* param) {
            respondOk();
        }

        void setHeader(const char* param) {
            _config.transmitAddress = core::convert::toUInt16(param, 16);
            respondOk();
        }

        void setCanReceiveAddress(const char* param) {
            _config.filterId = core::convert::toUInt16(param, 16);
            _can.filter(_config.filterId);
            _config.maskId = CANID_11_MAX;
            _can.mask(_config.maskId);
            respondOk();
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

            cstrings::removeWhitespaces(_command, input);
            cstrings::toUpper(_command);

            if (convert::isHexString(_command)) {
                sendToVehicle(_command);
            }
            else if (isCommand(_command, "ATRV", &param))
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
            else if (isCommand(_command, "ATNL", &param))
                normalLengthMessages(param);
            else if (isCommand(_command, "ATAT", &param))
                setAdaptiveTimingMode(param);
            else if (isCommand(_command, "ATPPS", &param))
                printProgramableParameters(param);
            else if (isCommand(_command, "ATI", &param))
                printVersion(param);
            else if (isCommand(_command, "ATWS", &param))
                warmStart(param);
            else if (isCommand(_command, "ATZ", &param))
                resetAll(param);
            else if (isCommand(_command, "AT@2", &param))
                respondUnkown();
            else if (isCommand(_command, "STI", &param))
                respondUnkown();
            else if (isCommand(_command, "VTI", &param))
                respondUnkown();
            else if (isCommand(_command, "ATPP", &param))
                respondOk();
            else if (isCommand(_command, "ATBI", &param))
                bypassInit(param);
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

        void initIsoTp() {
            _isotp.emplace(_dispatcher, _can, _config.transmitAddress, _config.filterId);
        }

        void deinitIsoTp() {
            _isotp.reset();
        }

    private:
        char _input[40] = {0};
        char _command[40] = {0};
        IDispatcher& _dispatcher;
        ElmRuntimeConfiguration _config;
        core::io::StreamReader _in;
        core::io::StreamWriter _out;
        core::StringBuilder _prompt;
        core::can::ICanInterface& _can;
        etl::optional<core::can::IsoTpSocket> _isotp;
        core::shared_ptr<IFuture> _backgroundWorkerTask;
        State _state;
        uint64_t _startedReceiving;
        bool _bytesReceived;

    };
}}}

#endif //UC_CORE_ELMCOMMANDINTERPRETER_H
