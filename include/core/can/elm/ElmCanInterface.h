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
#include <string>

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
            core::StringBuilder cmd;
            if (_txId != canFrame.id) {
                cmd.append("ATSH");
                std::string idStr = canIdToString(canFrame.id);
                cmd.append(idStr.c_str());
            }

            if (canFrame.isRemoteRequest) {
                cmd.append("RTR");
            } else {
                for (uint8_t i = 0; i < canFrame.length; i++) {
                    cmd.appendHex(canFrame.payload[i]);
                }
            }
            enqueCommand(cmd);

        }

        std::string canIdToString(canid_t canid) const {
            char idStr[5];
            StringBuilder id(idStr, 5);
            id.appendHex(canid);
            return std::string (id);
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
            _commandQueue.emplace(cmd);
        }

        void dispatchCommand() {

        }

    private:
        core::io::Stream& _elm;
        core::io::StreamWriter _elmOut;
        core::io::StreamReader _elmIn;
        canid_t _txId;
        etl::queue_spsc_atomic<std::string, 8> _commandQueue;
        char _response[64];
        bool _ready;
    };
}}}

#endif //SGLOGGER_ELMCANINTERFACE_H
