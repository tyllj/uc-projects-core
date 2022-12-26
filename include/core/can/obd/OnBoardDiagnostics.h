//
// Created by tyll on 2022-10-01.
//

#ifndef UC_CORE_ONBOARDDIAGNOSTICS_H
#define UC_CORE_ONBOARDDIAGNOSTICS_H

#include "core/Future.h"
#include "core/can/ICanInterface.h"
#include "core/can/IsoTpSocket.h"
#include "etl/optional.h"
#include "ObdRequest.h"
#include "PidDefinitions.h"
#include "ObdTroubleCode.h"

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


        OnBoardDiagnostics(core::IDispatcher& dispatcher, ICanInterface& can, canid_t ecuId = 0x07E0) : _dispatcher(dispatcher), _can(can), _ecuId(ecuId) {

        }

        auto getCurrentData(ObdRequest request) {
            uint8_t length = 1;
            uint8_t queryMsg[7] = {OBD_GET_CURRENT_DATA};

            for (uint8_t i = 0; i < request.count(); i++) {
                queryMsg[i + 1] = request.at(i).value().Pid;
                length++;
            }
            initTp(); // TODO: write an RAII wrapper which gets moved into lambda
            _isotp->send(queryMsg, length);

            auto t = core::async([this, request = request, finally = Defer([this]{deinitTp();})](){
                IsoTpSocket& isotp = this->_isotp.value();
                isotp.receiveAndTransmit();
                IsoTpPacket p;
                if (isotp.tryReceive(p) && p.getData()[0] == OBD_GET_CURRENT_DATA + 40) {
                    ObdRequest response = request;
                    size_t j = 1; // skip [0] byte, which is service id.
                    while (j < p.length()) {
                        uint8_t pid = p.at(j++);
                        uint8_t pidLength = request.getByPid(pid).value().DataLength;
                        ObdPidValue pidValue(pid, nullptr, pidLength);
                        for (uint8_t k = 0; k < pidLength; k++, j++)
                            pidValue[k] = p.at(j); // PIDs with length > 4 are handled safely by the [] operator, but will not yield useful results
                        response.update(pidValue);
                    }
                    this->deinitTp();
                    return yieldReturn<ObdRequest>(response);
                }
                return yieldDelay<ObdRequest>(50);
            });
            return etl::move(t);
        }

        auto getStoredTroubleCodes(ObdTroubleCode* destination, size_t limit) {
            uint8_t queryMsg[] = {OBD_GET_DTC};
            initTp();
            _isotp->send(queryMsg, sizeof(queryMsg));

            return core::async([this, destination = destination, limit = limit, finally = Defer([this]{deinitTp();})](){
                auto& isotp = this->_isotp.value();
                isotp.receiveAndTransmit();
                auto p = IsoTpPacket();
                if (isotp.tryReceive(p) && p.getData()[0] == OBD_GET_DTC + 40) {
                    size_t i = 0;
                    size_t j = 1; // skip first byte, which is service id.
                    while (j < p.length() && i < limit) {
                        auto a = p.getData()[j];
                        j++;
                        auto b = p.getData()[j];
                        j++;
                        destination[i++] = { a, b };
                    }
                    this->deinitTp();
                    return yieldReturn();
                }
                return yieldDelay(50);
            });
        }

        template<size_t n>
        auto getStoredTroubleCodes(ObdTroubleCode (&destination)[n]) {
            return getStoredTroubleCodes(destination, n);
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
        core::IDispatcher& _dispatcher;
        core::can::ICanInterface& _can;
        etl::optional<IsoTpSocket> _isotp;
        canid_t _ecuId;
    };
}}}

#endif //UC_CORE_ONBOARDDIAGNOSTICS_H
