//
// Created by tyll on 2022-09-11.
//

#ifndef SGLOGGER_ELMCANINTERFACE_H
#define SGLOGGER_ELMCANINTERFACE_H

#include "core/can/ICanInterface.h"
#include "core/io/Stream.h"
#include "core/io/StreamWriter.h"
#include "core/io/StreamReader.h"
#include "ElmAtCommands.h"
#include "core/StringBuilder.h"

namespace core { namespace can { namespace elm {
    class ElmCanInterface : public core::can::ICanInterface {
    public:
        ElmCanInterface(core::io::Stream &elm) :
            _elm(elm),
            _elmOut(elm),
            _elmIn(elm) {
            _elmOut.setNewLine(cstrings::CR);
        }

        ~ElmCanInterface() override = default;

        void filter(canid_t filter) final {

        }

        void mask(canid_t filter) final {

        }

        void writeFrame(CanFrame& canFrame) final {
            core::StringBuilder sb;
            for (uint8_t i = 0; i < canFrame.length; i++) {
                sb.appendHex(canFrame.payload[i]);
                sb.appendHex(' ');
            }
            enqueCommand(sb.toString());
        }

        bool tryReadFrame(CanFrame& outCanFrame) final {

        }

        bool isReady() const {
            return _ready;
        }

    private:
        void initialize() {
            enqueCommand("ATZ");
            // Set variable DLC
        }

        void enqueCommand(const char* cmd) {
            _ready = false;
            strncpy(_nextCommand, cmd, sizeof(_nextCommand));
        }

        void dispatchCommand() {
            _elmOut.writeLine(_nextCommand);
            _nextCommand[0] = '\0';
        }

    private:
        core::io::Stream& _elm;
        core::io::StreamWriter _elmOut;
        core::io::StreamReader _elmIn;
        char _nextCommand[64];
        char _response[64];
        bool _ready;
    };
}}}

#endif //SGLOGGER_ELMCANINTERFACE_H
