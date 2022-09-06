//
// Created by tyll on 2022-03-14.
//

#ifndef UC_CORE_CHANNEL_H
#define UC_CORE_CHANNEL_H

#include <stdint.h>
#include "etl/delegate.h"
#include "ICanInterface.h"
#include "IsoTpSocket.h"
#include "core/Event.h"

namespace core { namespace can {
    template<canid_t id>
    struct Message {
        static canid_t getCanId() { return id; }
    };

    template<typename TMessage>
    class CanChannel {
    public:
        explicit CanChannel(ICanInterface &canInterface) : _socket(canInterface, TMessage::getCanId()) {
            _packetReceivedEventHandler.set<&CanChannel<TMessage>::onPacketReceived>(*this);
            _socket.newData() += _packetReceivedEventHandler;
        }

        ~CanChannel() {
            _socket.newData() -= _packetReceivedEventHandler;
        }

        void publish(TMessage &message) {
            _socket.send(&message, sizeof(message));
        }

        void subscribe(const etl::delegate<void(TMessage&)>& receivedAction) {
            _subscriber = &receivedAction;
        }

        void unsubscribe() {
            _subscriber = nullptr;
        }

    private:
        void onPacketReceived(const core::shared_ptr<core::can::IsoTpPacket>& packet) {
            if (_subscriber != nullptr && packet.notNull()) {
                TMessage& msg = *reinterpret_cast<TMessage*>(packet->getData());
                (*_subscriber)(msg);
            }
        }

    private:
        core::can::IsoTpSocket _socket;
        etl::delegate<void(core::shared_ptr<core::can::IsoTpPacket>)> _packetReceivedEventHandler;
        etl::delegate<void(TMessage&)>* _subscriber;
    };

    template<typename TMessage>
    class ResponsiveCanChannel : public CanChannel<TMessage> {
    public:
        explicit ResponsiveCanChannel(ICanInterface &canInterface) :
            CanChannel<TMessage>(canInterface),
            _canInterface(canInterface),
            _rtrService(nullptr) {
            _frameReceivedEventHandler.set<&ResponsiveCanChannel<TMessage>::onFrameReceived>(*this);
            _canInterface.newData() += _frameReceivedEventHandler;
        }

        ~ResponsiveCanChannel() {
            _canInterface.newData() -= _frameReceivedEventHandler;
        }

        void publishRtrService(const etl::delegate<TMessage(void)>& responseFunc) {
            _rtrService = &responseFunc;
        }

        void unpublishRtrService() {
            _rtrService = nullptr;
        }

    private:
        void onFrameReceived(core::can::CanFrame frame) {
            if (frame.isRemoteRequest && _rtrService != nullptr) {
                TMessage response = (*_rtrService)();
                publish(response);
            }
        }

    private:
        core::can::ICanInterface& _canInterface;
        etl::delegate<void(core::can::CanFrame)> _frameReceivedEventHandler;
        etl::delegate<TMessage(void)>* _rtrService;
    };
}}
#endif //UC_CORE_CHANNEL_H
