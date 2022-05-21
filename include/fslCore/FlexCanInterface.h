/*
 * FlexCanInterface.h
 *
 *  Created on: Feb 21, 2022
 *      Author: tyll
 */

#ifndef HAL_FLEXCANINTERFACE_H_
#define HAL_FLEXCANINTERFACE_H_

#include <stdint.h>
#include <fsl_flexcan.h>
#include "core/shared_ptr.h"
#include "core/can/CanInterface.h"

#define RX_MESSAGE_BUFFER_NUM (9)
#define RX_RTR_BUFFER_NUM (10)
#define TX_MESSAGE_BUFFER_NUM (8)
namespace fslCore {
    class FlexCanInterface : public core::net::canbus::CanInterface {
    public:
        FlexCanInterface(CAN_Type *can, uint32_t baudRate = 500000U,
                         flexcan_clock_source_t clockSource = kFLEXCAN_ClkSrcOsc,
                         uint32_t clockFreq = CLOCK_GetFreq(kCLOCK_BusClk)) : _can(can) {
            FLEXCAN_GetDefaultConfig(&_canConfig);
            _canConfig.clkSrc = clockSource;
            _canConfig.baudRate = baudRate;
            FLEXCAN_Init(can, &_canConfig, clockFreq);

            // TODO: Setup Rx
            _mbConfig.format = kFLEXCAN_FrameFormatStandard;
            _mbConfig.type = kFLEXCAN_FrameTypeData;

            // Setup Tx
            FLEXCAN_SetTxMbConfig(can, TX_MESSAGE_BUFFER_NUM, true);
        }

        ~FlexCanInterface() {
            FLEXCAN_Deinit(can);
        }

        // TODO: Transfer interrupt handler, background loop with event-handling

        canid_t getFilter() override {
            return _filterId;
        }

        void setFilter(canid_t filter) override {
            _filterId = filter;
            _mbConfig.id = FLEXCAN_ID_STD(_filterId);
            FLEXCAN_SetRxMbConfig(can, RX_MESSAGE_BUFFER_NUM, &_mbConfig, true);
        }

        canid_t getFilterMask() override {
            return _filterMask;
        }

        void setFilterMask(canid_t filter) override {
            _filterMask = filter;
            FLEXCAN_SetRxMbGlobalMask(_can, FLEXCAN_ID_STD(_filterMask));
            return;
        }


        void send(core::shared_ptr<core::net::canbus::CanFrame> canFrame) override {
            send(*canFrame);
        }

        void send(core::net::canbus::CanFrame &canFrame) override {
            flexcan_frame_t nxpFrame;
            uint8_t messageBufferIndex = 8;
            convertToNxpFrame(nxpFrame, canFrame);
            FLEXCAN_TransferSendBlocking(_can, messageBufferIndex, nxpFrame);

            // TODO: Refactor to semi-blocking transfer, see flexcan_interrupt_transfer.c
        }

    private:
        void convertToNxpFrame(flexcan_frame_t &dest, core::net::canbus::CanFrame &src) {
            dest.format = kFLEXCAN_FrameFormatStandard;
            dest.id = FLEXCAN_ID_STD(src.id);
            dest.type = src.isRemoteRequest ? kFLEXCAN_FrameTypeRemote : kFLEXCAN_FrameTypeData;
            dest.lenght = src.length;
            dest.dataByte0 = src.payload[0];
            dest.dataByte1 = src.payload[1];
            dest.dataByte2 = src.payload[2];
            dest.dataByte3 = src.payload[3];
            dest.dataByte4 = src.payload[4];
            dest.dataByte5 = src.payload[5];
            dest.dataByte6 = src.payload[6];
            dest.dataByte7 = src.payload[7];
        }

        void convertToCoreFrame(core::net::canbus::CanFrame &dest, flexcan_frame_t &src) {
            dest.id = (canid_t) (((uint32_t) (src.id)) >> CAN_ID_STD_SHIFT);  // reversal of FLEXCAN_ID_STD macro
            dest.isRemoteRequest = src.type == kFLEXCAN_FrameTypeRemote;
            dest.length = src.length;
            dest.payload[0] = src.dataByte0;
            dest.payload[1] = src.dataByte1;
            dest.payload[2] = src.dataByte2;
            dest.payload[3] = src.dataByte3;
            dest.payload[4] = src.dataByte4;
            dest.payload[5] = src.dataByte5;
            dest.payload[6] = src.dataByte6;
            dest.payload[7] = src.dataByte7;
        }

    private:
        flexcan_config_t _canConfig;
        flexcan_rx_mb_config_t _mbConfig;
        canid_t _filterId;
        canid_t _filterMask;
        const CAN_Type *_can;

        volatile bool txComplete = false;
        volatile bool rxComplete = false;

        flexcan_frame_t txFrame, rxFrame;

        flexcan_handle_t flexcanHandle;
        flexcan_mb_transfer_t txXfer, rxXfer;
    };
}

#endif /* HAL_FLEXCANINTERFACE_H_ */
