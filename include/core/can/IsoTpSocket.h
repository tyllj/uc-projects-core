//
// Created by tyll on 2022-01-31.
//

#ifndef SGLOGGER_ISOTPSOCKET_H
#define SGLOGGER_ISOTPSOCKET_H

#include <stdint.h>
#include <string.h>
#include "etl/delegate.h"
#include "core/events/Observable.h"
#include "core/can/CanInterface.h"

namespace core { namespace can {
    enum IsoTpHeaderType {
        ISOTP_SINGLE = 0,
        ISOTP_FIRST = 1,
        ISOTP_CONSECUTIVE = 2,
        ISOTP_FLOW_CONTROL = 3
    };

    enum IsoTpFlowControlType {
        ISOTP_CONTINUE = 0,
        ISOTP_WAIT = 1,
        ISOTP_OVERFLOW = 2,
        ISOTP_NO_FC = 255
    };

    union IsoTpFrame {
    public:
        IsoTpFrame(CanFrame canData) : _canData(canData) {}
        IsoTpHeaderType getHeaderType() {
            return (IsoTpHeaderType)((_canData.payload[0] >> 4) & 0x0F);
        }
        uint16_t getMessageLength() {
            switch (getHeaderType()) {
                case ISOTP_SINGLE: return _canData.payload[0] & 0x0F;
                case ISOTP_FIRST: return (((uint16_t)(_canData.payload[0] & 0x0F)) << 8) | (uint16_t)_canData.payload[1];
                default: return 0;
            }
        }
        uint8_t getIndex() {
            if (getHeaderType() == ISOTP_CONSECUTIVE)
                return _canData.payload[0] & 0x0F;
            return 0;
        }
        uint8_t getDataByteCount() {
            switch (getHeaderType()) {
                case ISOTP_SINGLE: return 7;
                case ISOTP_FIRST: return 6;
                case ISOTP_CONSECUTIVE: return 7;
                default: return 0;
            }
        }
        uint8_t* getPayload() {
            switch (getHeaderType()) {
                case ISOTP_SINGLE: return &_canData.payload[1];
                case ISOTP_FIRST: return &_canData.payload[2];
                case ISOTP_CONSECUTIVE: return &_canData.payload[1];
                default: return _canData.payload;
            }
        }
        IsoTpFlowControlType getFlowControlType() {
            if (getHeaderType() == ISOTP_FLOW_CONTROL)
                return (IsoTpFlowControlType)(_canData.payload[0] & 0x0F);
            return ISOTP_NO_FC;
        }
        uint8_t getSeparationTimeMillis() {
            return 0;
        } //TODO: Handle 100x µS delays.
    private:
        CanFrame _canData;
    };

    class IsoTpPacket {
    public:
        explicit IsoTpPacket(size_t length) : _length(length), _position(0) {
            _data = new uint8_t[length];
        }

        ~IsoTpPacket() {
            delete[] _data;
        }

        void appendFrame(IsoTpFrame& frame) {
            uint8_t* p = frame.getPayload();
            for (uint16_t i = 0; _position < _length && i < frame.getDataByteCount(); _position++, i++) {
                _data[_position] = p[i];
            }
        }

        inline uint8_t* getData() {
            return _data;
        }

        inline bool isFull() {
            return _position == _length;
        }

    private:
        size_t _length;
        size_t _position;
        uint8_t* _data;
    };

    class IsoTpSocket : public events::Observable<shared_ptr<IsoTpPacket>>{
    public:

        explicit IsoTpSocket(CanInterface& canInterface, canid_t canId) : _canInterface(canInterface), _canId(canId) {
            canFrameReceived.set<IsoTpSocket, &IsoTpSocket::onDataReceived>(*this);
            _canInterface.newData() += canFrameReceived;
        }

        ~IsoTpSocket() {
            _canInterface.newData() -= canFrameReceived;
        }

        void send(uint8_t* data, uint16_t length) {
            uint16_t position = 0;
            uint8_t index = 0;
            IsoTpHeaderType packageType = length <= 7 ? ISOTP_SINGLE : ISOTP_FIRST;
            length &= 0x0FFF;
            while (position < length) {
                CanFrame frame;
                setDefaultBytes(frame.payload);
                frame.id = _canId;
                switch (packageType) {
                    case ISOTP_SINGLE:
                        frame.payload[0] = length;
                        copy(position, length, 7, &data[position], &frame.payload[1]);
                        break;
                    case ISOTP_FIRST:
                        frame.payload[0] = (uint8_t) (0x10 | (length >> 8));
                        frame.payload[1] = (uint8_t) (length & 0x00FF);
                        copy(position, length, 6, &data[position], &frame.payload[2]);
                        break;
                    case ISOTP_CONSECUTIVE:
                        frame.payload[0] = 0x20 | (index & 0x0F);
                        copy(position, length, 7, &data[position], &frame.payload[1]);
                        break;
                }
                _canInterface.send(frame);
                index++;
                packageType = ISOTP_CONSECUTIVE;
            }
        }

    private:
            void copy(uint16_t& position, uint16_t& length, uint8_t bytes, uint8_t* src, uint8_t* dst) {
                for (uint16_t i = 0; i < 6 && position < length; position++, i++) {
                    dst[i] = src[i];
                }
            }

            void setDefaultBytes(uint8_t* payload) {
                for (uint8_t i = 0; i < 8; i++)
                    payload[i] = 0xCC;
            }

            void discardPacket() {
                if (packet.notNull())
                    packet = core::shared_ptr<IsoTpPacket>();
            }

        void onDataReceived(CanFrame canFrame) {
            IsoTpFrame& f = reinterpret_cast<IsoTpFrame&>(canFrame);
            bool isMessageStart = f.getHeaderType() == ISOTP_SINGLE || f.getHeaderType() == ISOTP_FIRST;
            bool isMultiPartMessage = f.getHeaderType() == ISOTP_FIRST || f.getHeaderType() == ISOTP_CONSECUTIVE;
            bool isContinuation = f.getHeaderType() == ISOTP_CONSECUTIVE;
            bool isFlowControl = f.getHeaderType() == ISOTP_FLOW_CONTROL;

            if (isMessageStart) {
                packet = shared_ptr<IsoTpPacket>(new IsoTpPacket(f.getDataByteCount())); // overwrites any started packet
                packet->appendFrame(f);
            } else if (isContinuation && packet.notNull()) {
                packet->appendFrame(f);
            } else if (isFlowControl) {
                // TODO
            } else {
                // invalid packet
                discardPacket();
            }

            if (packet.isNull())
                return;

            if (packet->isFull()) {
                //core::shared_ptr<core::can::canbus::IsoTpPacket> p = packet;
                this->notify(core::shared_ptr<core::can::IsoTpPacket>(packet));
                discardPacket(); // Event listener must have taken ownership now.
            } else if (isMultiPartMessage && isMessageStart) {
                sendContinueAllRequest();
            }
        }

        void sendContinueAllRequest() {
            CanFrame frame;
            frame.id = _canId;
            frame.payload[0] = 0x30; // 0x30 = ISOTP flow control + 0x00 = continue to send
            _canInterface.send(frame);
        }

        CanInterface& _canInterface;
        const canid_t _canId;
        shared_ptr<IsoTpPacket> packet;
        etl::delegate<void(core::can::CanFrame)> canFrameReceived;

    };
}}
#endif //SGLOGGER_ISOTPSOCKET_H
