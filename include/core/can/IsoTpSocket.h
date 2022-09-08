//
// Created by tyll on 2022-01-31.
//

#ifndef UC_CORE_ISOTPSOCKET_H
#define UC_CORE_ISOTPSOCKET_H

#include <stdint.h>
#include <string.h>
#include "etl/utility.h"
#include "etl/queue_spsc_atomic.h"
#include "core/unique_ptr.h"
#include "core/can/ICanInterface.h"
#include "core/shared_ptr.h"
#include "core/async/Future.h"

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

    enum IsoTpSeparationDelayUnit {
        ISOTP_MICROS,
        ISOTP_MILLIS
    };

    struct IsoTpSeparationDelay {
    public:
        uint16_t rawValue;
        IsoTpSeparationDelayUnit unit() {

        }
        uint16_t value() {

        }

    };

    union IsoTpFrame {
    public:
        IsoTpFrame(CanFrame canData) : _canData(canData) {}
        IsoTpHeaderType getHeaderType() {
            return static_cast<IsoTpHeaderType>((_canData.payload[0] >> 4) & 0x0F);
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
                return static_cast<IsoTpFlowControlType>(_canData.payload[0] & 0x0F);
            return ISOTP_NO_FC;
        }
        uint8_t getSeparationTime() {
            return 0;
        } //TODO: Handle 100x µS delays.
    private:
        CanFrame _canData;
    };

    class IsoTpPacket {
    public:
        IsoTpPacket() : _length(0), _position(0), _data(core::unique_ptr<uint8_t[]>()) {}

        explicit IsoTpPacket(size_t length) : _length(length), _position(0), _data(core::unique_ptr<uint8_t[]>(new uint8_t[length])) {

        }

        void appendFrame(IsoTpFrame& frame) {
            _sequenceId = frame.getIndex();
            _separationDelay = frame.getSeparationTime();
            if (frame.)
            uint8_t* p = frame.getPayload();
            for (uint16_t i = 0; _position < _length && i < frame.getDataByteCount(); _position++, i++) {
                _data[_position] = p[i];
            }
        }

        inline size_t length() { return _length; }

        inline uint8_t* getData() {
            return _data.get();
        }

        inline bool isFull() {
            return _position == _length;
        }

    private:
        size_t _length;
        size_t _position;
        uint16_t _separationDelay;
        uint8_t _sequenceId;
        core::unique_ptr<uint8_t[]> _data;
    };

#ifndef UC_CORE_ISOTP_QUEUE_SIZE
#define UC_CORE_ISOTP_QUEUE_SIZE 8
#endif

    class IsoTpSocket {
    private:
        struct BackgroundTaskContext {
            IsoTpSocket* socket;
            core::shared_ptr<bool> disposedFlag;
        };
    public:
        explicit IsoTpSocket(core::async::IDispatcher& dispatcher, core::can::ICanInterface& canInterface, canid_t canId, size_t maxPayload = 128) :
                _canInterface(canInterface),
                _txId(canId),
                _disposedFlag(core::shared_ptr<bool>(new bool{false})),
                _maxPayload(maxPayload),
                _bufferedPayload(0) {
            startBackgroundWorker(dispatcher);
        }

        void startBackgroundWorker(async::IDispatcher &dispatcher) const {
            etl::delegate<void(async::FutureContext<BackgroundTaskContext, void*>&)> backgroundTaskDelegate;
            backgroundTaskDelegate.set([=](async::FutureContext<BackgroundTaskContext, void*> ctx) {
                if (ctx.getData().disposedFlag.unique())
                    FUTURE_RETURN(ctx, nullptr);
                ctx.getData().socket->backgroundReceive();
            });

            async::Future<BackgroundTaskContext, void*> backgroundTask({const_cast<IsoTpSocket*>(this), _disposedFlag}, backgroundTaskDelegate);
            dispatcher.run(shared_ptr<async::IFuture>(new async::Future<BackgroundTaskContext, void*> {backgroundTask}));
        }

        void send(uint8_t* data, uint16_t length) {
            uint16_t position = 0;
            uint8_t index = 0;
            IsoTpHeaderType packageType = length <= 7 ? ISOTP_SINGLE : ISOTP_FIRST;
            length &= 0x0FFF;
            while (position < length) {
                CanFrame frame;
                setDefaultBytes(frame.payload);
                frame.id = _txId;
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
                    default:
                        break;
                }
                _canInterface.writeFrame(frame);
                index++;
                packageType = ISOTP_CONSECUTIVE;
            }
        }

        bool available() const { return !_received.empty(); }

        bool tryReceive(IsoTpPacket& outPacket) {
            if (!available())
                return false;
            _received.pop(outPacket);
            _bufferedPayload -= outPacket.length();
            return true;
        }

    private:
        void copy(uint16_t& position, uint16_t& length, uint8_t bytes, uint8_t* src, uint8_t* dst) {
            for (uint16_t i = 0; i < bytes && position < length; position++, i++) {
                dst[i] = src[i];
            }
        }

        void setDefaultBytes(uint8_t* payload) {
            for (uint8_t i = 0; i < 8; i++)
                payload[i] = 0xCC;
        }

        void discardPacket() {
            if (_packet.getData() != nullptr) {
                _bufferedPayload -= _packet.length();
                _packet = IsoTpPacket();
            }
        }

        void backgroundReceive() {
            CanFrame canFrame;
            if (_canInterface.tryReadFrame(canFrame) && canFrame.id == _rxId)
                onDataReceived(canFrame);
        }

        void onDataReceived(CanFrame& canFrame) {
            IsoTpFrame& f = reinterpret_cast<IsoTpFrame&>(canFrame);
            bool isMessageStart = f.getHeaderType() == ISOTP_SINGLE || f.getHeaderType() == ISOTP_FIRST;
            bool isMultiPartMessage = f.getHeaderType() == ISOTP_FIRST || f.getHeaderType() == ISOTP_CONSECUTIVE;
            bool isContinuation = f.getHeaderType() == ISOTP_CONSECUTIVE;
            bool isFlowControl = f.getHeaderType() == ISOTP_FLOW_CONTROL;

            if (isMessageStart) {
                size_t recvLength = f.getDataByteCount();
                if (_bufferedPayload + recvLength > _maxPayload)
                    return;
                _packet = IsoTpPacket(recvLength); // overwrites any started packet
                _packet.appendFrame(f);
                _bufferedPayload += recvLength;
            } else if (isContinuation && !_packet.isFull()) {
                _packet.appendFrame(f);
            } else if (isFlowControl) {
                //ToDo
            } else {
                // invalid packet
                discardPacket();
            }

            if (_packet.getData() == nullptr)
                return;

            if (_packet.isFull()) {
                while (!_received.push(etl::move(_packet)));
            } else if (isMultiPartMessage && isMessageStart) {
                sendContinueAllRequest();
            }
        }

        void sendContinueAllRequest() const {
            CanFrame frame;
            frame.id = _txId;
            frame.payload[0] = 0x30; // 0x30 = ISOTP flow control + 0x00 = continue to send
            _canInterface.writeFrame(frame);
        }

        ICanInterface& _canInterface;
        canid_t _txId;
        canid_t _rxId;
        IsoTpPacket _packet;
        core::shared_ptr<bool> _disposedFlag;
        size_t _maxPayload;
        size_t _bufferedPayload;
        etl::queue_spsc_atomic<IsoTpPacket, UC_CORE_ISOTP_QUEUE_SIZE> _received;
    };
}}
#endif //UC_CORE_ISOTPSOCKET_H
