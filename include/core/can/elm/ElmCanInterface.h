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
            std::string idStr = canIdToString(_txId);
            core::StringBuilder cmd;
            cmd.append("ATCF");
            cmd.append(idStr.c_str());
            enqueCommand(cmd);
        }

        void mask(canid_t filter) final {
            std::string idStr = canIdToString(_txId);
            core::StringBuilder cmd;
            cmd.append("ATCM");
            cmd.append(idStr.c_str());
            enqueCommand(cmd);
        }

        void writeFrame(CanFrame& canFrame) final {
            core::StringBuilder cmd;
            if (_txId != canFrame.id) {
                _txId = canFrame.id;
                std::string idStr = canIdToString(_txId);
                cmd.append("ATSH");
                cmd.append(idStr.c_str());
                enqueCommand(cmd);
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
            return {id };
        }

        bool tryReadFrame(CanFrame& outCanFrame) final {

        }

    private:
        void initialize() {
            enqueCommand("ATZ"); // reset
            // Echo off
            // DIsable autoformatting
            // Set variable DLC
            // Disable flow control
            // Disable silent monitoring
            // Enable Monitor all messages
            // Bypass initialization sequence
        }

        void enqueCommand(const char* cmd) {
            _ready = false;
            _commandQueue.emplace(cmd);
        }

        void dispatchCommand() {
            if (_commandQueue.empty())
                return;
            std::string cmd;
            _commandQueue.pop(cmd);
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
