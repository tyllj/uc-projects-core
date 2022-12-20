//
// Created by tyll on 2022-01-31.
//

#ifndef UC_CORE_CANINTERFACE_H
#define UC_CORE_CANINTERFACE_H

#include <stdint.h>


typedef uint16_t canid_t;
constexpr canid_t CANID_11_MAX = 0x7FF;

namespace core { namespace can {
    /*
     * CAN Bus filter Rules:

  The filter mask is used to determine which bits in the identifier of the received frame are compared with the filter

   If a mask bit is set to a zero, the corresponding ID bit will automatically be accepted, regardless of the value of the filter bit.

   If a mask bit is set to a one, the corresponding ID bit will be compare with the value of the filter bit; if they match it is accepted otherwise the frame is rejected.
     */

#pragma pack(push, 1)
    struct CanFrame {
        canid_t id;
        bool isRemoteRequest = false; // value of the rtr-bit
        uint8_t length = 8; // value of the dlc
        uint8_t payload[8];
    };
#pragma pack(pop)

    class ICanInterface {
    public:
        virtual void filter(canid_t filter) = 0;
        virtual void mask(canid_t filter) = 0;
        virtual void writeFrame(CanFrame& canFrame) = 0;
        virtual bool tryReadFrame(CanFrame& outCanFrame) = 0;
        virtual ~ICanInterface() = default;
    };
}}

#endif //UC_CORE_CANINTERFACE_H
