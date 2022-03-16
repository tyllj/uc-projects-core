//
// Created by tyll on 2022-01-31.
//

#ifndef SGLOGGER_CANINTERFACE_H
#define SGLOGGER_CANINTERFACE_H

#include <stdint.h>
#include "core/events/Observable.h"
#include "core/shared_ptr.h"

typedef uint16_t canid_t;

namespace core { namespace can {
    /*
     * CAN Bus filter Rules:

  The filter mask is used to determine which bits in the identifier of the received frame are compared with the filter

   If a mask bit is set to a zero, the corresponding ID bit will automatically be accepted, regardless of the value of the filter bit.

   If a mask bit is set to a one, the corresponding ID bit will be compare with the value of the filter bit; if they match it is accepted otherwise the frame is rejected.
     */

    struct CanFrame {
        canid_t id;
        bool isRemoteRequest = false; // value of the rtr-bit
        uint8_t length = 8; // value of the dlc
        uint8_t payload[8];
    };

    class CanInterface : public events::Observable<CanFrame> {
    public:
        virtual canid_t getFilter() = 0;
        virtual void setFilter(canid_t filter) = 0;
        virtual canid_t getFilterMask() = 0;
        virtual void setFilterMask(canid_t filter) = 0;
        virtual void send(CanFrame& canFrame) = 0;
        virtual void send(shared_ptr<CanFrame> canFrame) = 0;
    };
}}

#endif //SGLOGGER_CANINTERFACE_H
