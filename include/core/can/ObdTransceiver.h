//
// Created by tyll on 2022-02-04.
//

#ifndef UC_CORE_OBDTRANSCEIVER_H
#define UC_CORE_OBDTRANSCEIVER_H

#include "core/Math.h"
#include "core/async/Eventual.h"
#include "core/can/CanInterface.h"
#include "core/can/IsoTpSocket.h"
#include "core/can/ObdData.h"

namespace core { namespace can {
    class ObdTransceiver {
    public:
        explicit ObdTransceiver(CanInterface& canInterface) :
            _pendingResponse(nullptr),
            _canInterface(canInterface),
            _responseSocket(canInterface, 0x7DF) {
            _packetReceivedCallback.set<ObdTransceiver, &ObdTransceiver::onResponseReceived>(*this);
        }

        ~ObdTransceiver() {
            if (isRequestPending())
                terminateRequest();
        }

        // Caller provides memory for response. Object referenced by response must outlive the request cycle!
        void requestAsync(MultiRequest& request, core::async::Eventual<MultiResponse>& response) {
            _request = request; // request PID list is copied into instance variable
            _pendingResponse = &response;
            _pendingResponse->reset();
            _responseSocket.newData() += _packetReceivedCallback;
        }

        shared_ptr<core::async::Eventual<MultiResponse>> requestAsync(MultiRequest& request) {
            shared_ptr<core::async::Eventual<MultiResponse>> response(new core::async::Eventual<MultiResponse>());
            this->requestAsync(request, *response);
            return response;
        }

        bool isRequestPending() {
            return _pendingResponse != nullptr;
        }

        void terminateRequest() {
            _responseSocket.newData() -= _packetReceivedCallback;
            _pendingResponse = nullptr;
        }

    private:
        void onResponseReceived(shared_ptr<IsoTpPacket> packet) {
            if (_pendingResponse) {
                MultiResponse response;
                parseResponse(packet, _request, response);
                _pendingResponse->set(response);
                terminateRequest(); // Unset event handlers and discard references, now that the request is processed.
            }
        }

        void parseResponse(shared_ptr<IsoTpPacket> packet, MultiRequest& request, MultiResponse& response) {
            uint8_t* data = packet->getData();
            size_t ptr = 0;

            for (uint8_t i = 0; i < request.length(); i++) {
                uint8_t actualPid = data[ptr++];
                if (!request.containsPid(actualPid)) {
                    // Error: received PID was not requested.
                    return;
                }

                uint32_t rawValue = 0;
                // only read first four bytes per pid, skip remaining bits
                uint8_t valueLength = request.pid(actualPid).responseLength;
                uint8_t readLength = min((uint8_t) 4, valueLength);
                uint8_t shift = 24;
                for (uint8_t j = readLength; j > 0; j--) {
                    rawValue |= ((uint32_t) data[ptr++]) << shift;
                    shift -= 8;
                }

                ptr += valueLength - readLength;
                response.add(actualPid, ObdValue(rawValue));
            }
        }

    private:
        MultiRequest _request;
        core::async::Eventual<MultiResponse>* _pendingResponse;
        CanInterface& _canInterface;
        IsoTpSocket _responseSocket;
        etl::delegate<void(shared_ptr<core::can::IsoTpPacket>)> _packetReceivedCallback;
    };
}}

#endif //UC_CORE_OBDTRANSCEIVER_H
