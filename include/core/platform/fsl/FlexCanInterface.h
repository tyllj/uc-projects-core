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
#include "core/can/ICanInterface.h"
#include "core/Future.h"

#define RX_MESSAGE_BUFFER_NUM (9)
#define RX_RTR_BUFFER_NUM (10)
#define TX_MESSAGE_BUFFER_NUM (8)

extern "C" inline void flexcan_callback(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *instance);

namespace fslCore {
	enum FlexCanOperationMode {
		FLEXCAN_NORMAL,
		FLEXCAN_LOOPBACK
	};

    class FlexCanInterface : public core::can::ICanInterface {
    public:
        FlexCanInterface(core::coop::IDispatcher& dispatcher, CAN_Type *can, uint32_t baudRate = 1000000U, FlexCanOperationMode mode = FLEXCAN_NORMAL,
                         flexcan_clock_source_t clockSource = kFLEXCAN_ClkSrcPeri,
                         uint32_t clockFreq = CLOCK_GetFreq(kCLOCK_BusClk)) : _dispatcher(dispatcher), _can(can) {
            FLEXCAN_GetDefaultConfig(&_canConfig);
            _canConfig.clkSrc = clockSource;
            _canConfig.baudRate = baudRate;
            _canConfig.enableLoopBack = mode == FLEXCAN_LOOPBACK;
            _canConfig.enableSelfWakeup = true;
            FLEXCAN_Init(can, &_canConfig, clockFreq);

            // TODO: Setup Rx
            _mbConfig.format = kFLEXCAN_FrameFormatStandard;
            _mbConfig.type = kFLEXCAN_FrameTypeData;
            _rxXfer.frame = &_rxFrame;
            _rxXfer.mbIdx = RX_MESSAGE_BUFFER_NUM;
            _txXfer.frame = &_txFrame;
            _txXfer.mbIdx = TX_MESSAGE_BUFFER_NUM;

            // Setup Tx
            FLEXCAN_SetTxMbConfig(can, TX_MESSAGE_BUFFER_NUM, true);
            FLEXCAN_EnableMbInterrupts(can, 1 << TX_MESSAGE_BUFFER_NUM);

            FLEXCAN_TransferCreateHandle(can, &_flexcanHandle, flexcan_callback, (void*) this);

            setTxComplete();
            core::shared_ptr<core::coop::IFuture> transferTask(new core::coop::Future<FlexCanInterface*, void*>(this, [](core::coop::FutureContext<FlexCanInterface*, void*> ctx) {
				FlexCanInterface& self = *(ctx.getData());
            	if (self.isRxComplete()) {
            		// Step 1: Handle Packet, throw event
					core::can::CanFrame outFrame;
					fslCore::FlexCanInterface::convertToCoreFrame(outFrame, self._rxFrame);
					self.notify(outFrame);
            		// Step 2: Restart TransferReceive.
            		self.resetRxComplete();
            		FLEXCAN_TransferReceiveNonBlocking(self._can, &(self._flexcanHandle), &(self._rxXfer));
            	}
            }));
    		FLEXCAN_TransferReceiveNonBlocking(can, &_flexcanHandle, &_rxXfer);

            dispatcher.run(transferTask); // TODO: Stop task in destructor. Maybe introduce dispatcher.cancel(shared_ptr<IFuture>) function?
        }

        ~FlexCanInterface() {
            FLEXCAN_Deinit(_can);
        }

        // TODO: Transfer interrupt handler, background loop with event-handling

        void filter(canid_t filter) final {
            _filterId = filter;
            _mbConfig.id = FLEXCAN_ID_STD(_filterId);
            FLEXCAN_SetRxMbConfig(_can, RX_MESSAGE_BUFFER_NUM, &_mbConfig, true);
        }

        void mask(canid_t filter) final {
            _filterMask = filter;
            FLEXCAN_SetRxMbGlobalMask(_can, FLEXCAN_ID_STD(_filterMask));
            FLEXCAN_SetRxIndividualMask(_can, RX_MESSAGE_BUFFER_NUM, FLEXCAN_ID_STD(_filterMask));
            return;
        }

        void writeFrame(core::can::CanFrame &canFrame) final {
        	while (!isTxComplete());
        	resetTxComplete();
            uint8_t messageBufferIndex = TX_MESSAGE_BUFFER_NUM;
            convertToNxpFrame(_txFrame, canFrame);
            //FLEXCAN_TransferSendBlocking(_can, messageBufferIndex, &nxpFrame);

            FLEXCAN_TransferSendNonBlocking(_can, &_flexcanHandle, &_txXfer);
            // TODO: Refactor to semi-blocking transfer, see flexcan_interrupt_transfer.c
        }

        // TODO: Implement tryReadFrame method. Add a queue perhaps?


        /* Functions below should only be used internally */
        void setRxComplete() { _rxComplete = true; }
        void setTxComplete() { _txComplete = true; }

    private:

        void resetRxComplete() { _rxComplete = false; }
		void resetTxComplete() { _rxComplete = false; }

		bool isRxComplete() { return _rxComplete; }
		bool isTxComplete() { return _txComplete; }

        static void convertToNxpFrame(flexcan_frame_t &dest, core::can::CanFrame &src) {
            dest.format = kFLEXCAN_FrameFormatStandard;
            dest.id = FLEXCAN_ID_STD(src.id);
            dest.type = src.isRemoteRequest ? kFLEXCAN_FrameTypeRemote : kFLEXCAN_FrameTypeData;
            dest.length = src.length;
            dest.dataByte0 = src.payload[0];
            dest.dataByte1 = src.payload[1];
            dest.dataByte2 = src.payload[2];
            dest.dataByte3 = src.payload[3];
            dest.dataByte4 = src.payload[4];
            dest.dataByte5 = src.payload[5];
            dest.dataByte6 = src.payload[6];
            dest.dataByte7 = src.payload[7];
        }

        static void convertToCoreFrame(core::can::CanFrame &dest, flexcan_frame_t &src) {
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
        core::coop::IDispatcher& _dispatcher;
        flexcan_config_t _canConfig;
        flexcan_rx_mb_config_t _mbConfig;
        canid_t _filterId;
        canid_t _filterMask;
        CAN_Type *_can;

        volatile bool _txComplete = false;
        volatile bool _rxComplete = false;

        flexcan_frame_t _txFrame;
		flexcan_frame_t _rxFrame;

        flexcan_handle_t _flexcanHandle;
        flexcan_mb_transfer_t _txXfer;
		flexcan_mb_transfer_t _rxXfer;
    };


}

extern "C" inline  void flexcan_callback(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *instance) {
	fslCore::FlexCanInterface& self = *reinterpret_cast<fslCore::FlexCanInterface*>(instance);
	switch (status) {
		case kStatus_FLEXCAN_RxIdle:
			if (RX_MESSAGE_BUFFER_NUM == result) {
				self.setRxComplete();
			}
			break;

		case kStatus_FLEXCAN_TxIdle:
			if (TX_MESSAGE_BUFFER_NUM == result) {
				self.setTxComplete();
			}
			break;

		default:
			break;
	}
}

#endif /* HAL_FLEXCANINTERFACE_H_ */
