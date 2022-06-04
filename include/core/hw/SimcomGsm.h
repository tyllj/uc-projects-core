//
// Created by tyll on 2022-05-16.
//

#ifndef SGLOGGER_SIMCOMGSM_H
#define SGLOGGER_SIMCOMGSM_H

#include "core/io/Stream.h"
#include "core/io/StreamReader.h"
#include "core/io/StreamWriter.h"
#include "core/async/Future.h"

namespace core { namespace hw {
    enum GsmState {
        GPRS_NO_CONNECT,
        GPRS_READY,
        GPRS_HTTP_READY,
        GPRS_HTTP_RESPONSE_PENDING

    };

    class SimcomGsm {
    public:
        explicit SimcomGsm(core::io::Stream& uart) : _uart(uart), _reader(uart), _writer(uart) {

        }

        ~SimcomGsm() {

        }

    private:
        uint64_t millis() {
            return 0; // TODO
        }

        void sendCommand(GsmState requiredState, GsmState targetState, const char *command, unsigned long timeout) {
            if (_state != requiredState)
                return;

            _writer.flush();
            _commandOutputPosition = 0;

            _writer.writeLine(command);
            _commandTimeout = timeout;
            _commandStartTime = millis();
            _state = targetState;
        }

        bool continueReceiveResponse() {
            if ((millis() - _commandStartTime) < _commandTimeout) {
                _commandOutputPosition += _uart.read(reinterpret_cast<uint8_t *>(_commandOutput),
                                                     _commandOutputPosition,
                                                     _commandOutputSize - _commandOutputPosition);
                return true;
            }
            return false;
        }

        void finalizeReceivedResponse() {
            _commandOutput[_commandOutputPosition] = '\0';
        }

        async::Future<SimcomGsm*, bool> sendAndAssert() {
            return async::Future<SimcomGsm*, bool>(this, [](core::async::FutureContext<SimcomGsm*, bool>& ctx) {
                SimcomGsm& self = *(ctx.getData());
                self.continueReceiveResponse();
            }); // TODO: continueWith( self -> self.finalizeReceivedResponse() ) ...
        }


    private:
        core::io::Stream& _uart;
        core::io::StreamReader _reader;
        core::io::StreamWriter _writer;
        char _commandOutput[256];
        size_t _commandOutputSize = 256;
        size_t _commandOutputPosition;
        uint64_t _commandTimeout;
        uint64_t _commandStartTime;
        GsmState _state;

    };
}}

#endif //SGLOGGER_SIMCOMGSM_H
