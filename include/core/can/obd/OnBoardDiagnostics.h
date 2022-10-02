//
// Created by tyll on 2022-10-01.
//

#ifndef UC_CORE_ONBOARDDIAGNOSTICS_H
#define UC_CORE_ONBOARDDIAGNOSTICS_H

#include "core/coop/Future.h"
#include "core/can/ICanInterface.h"
#include "core/can/IsoTpSocket.h"
#include "etl/optional.h"
#include "ObdRequest.h"
#include "PidDefinitions.h"

namespace core { namespace can { namespace obd {

    constexpr uint8_t OBD_GET_CURRENT_DATA = 0x01;
    constexpr uint8_t OBD_GET_FREEZE_FRAME = 0x02;
    constexpr uint8_t OBD_GET_DTC = 0x03;
    constexpr uint8_t OBD_CLEAR_DTC = 0x04;
    constexpr uint8_t OBD_TEST_RESULTS = 0x06;
    constexpr uint8_t OBD_GET_PENDING_DTC = 0x07;
    constexpr uint8_t OBD_VEHICLE_INFORMATION = 0x09;
    constexpr uint8_t OBD_GET_PERMAMENT_DTC = 0x0A;

    class OnBoardDiagnostics {
    public:
        OnBoardDiagnostics(core::coop::IDispatcher& dispatcher, ICanInterface& can, canid_t ecuId = 0x07E0) : _dispatcher(dispatcher), _can(can), _ecuId(ecuId) {

        }

        core::coop::Future<OnBoardDiagnostics*, ObdRequest> getCurrentData(ObdRequest request) {
            uint8_t queryMsg[7] = {0};
            queryMsg[0] = OBD_GET_CURRENT_DATA;
            uint8_t i = 0;
            for (; i < request.count(); i++)
                queryMsg[i + 1] = request.at(i).Pid;
            initTp();
            _isotp->send(queryMsg, i + 1);

            core::coop::Future<OnBoardDiagnostics*, ObdRequest> future = {this, [](core::coop::FutureContext<OnBoardDiagnostics*, ObdRequest>& ctx) {
                OnBoardDiagnostics& self = *ctx.getData();

                IsoTpSocket& isotp = self._isotp.value();
                IsoTpPacket p;
                if (isotp.tryReceive(p)) {
                    uint8_t j = 0;
                    ObdRequest response;
                    while (j < p.length()) {
                        ObdValue pid = {p.getData()[j++]};
                        for (uint8_t k = 0; k > getDataLengthForPid(pid.Pid); k++)
                            pid[k] = p.getData()[j++];
                        response.add(pid);
                    }
                    self.deinitTp();
                    FUTURE_RETURN(ctx, response);
                }
            }};

            return future;
        }

    private:
        void initTp() {
            canid_t responseId = _ecuId + 8;
            _can.filter(responseId);
            _can.mask(CANID_11_MAX);
            _isotp.emplace(_dispatcher, _can, _ecuId, responseId);
        }

        void deinitTp() {
            _isotp.reset();
        }

    private:
        core::coop::IDispatcher& _dispatcher;
        core::can::ICanInterface& _can;
        etl::optional<IsoTpSocket> _isotp;
        canid_t _ecuId;
    };
}}}

#endif //UC_CORE_ONBOARDDIAGNOSTICS_H
