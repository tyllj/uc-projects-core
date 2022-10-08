//
// Created by tyll on 2022-10-01.
//

#ifndef UC_CORE_OBDREQUEST_H
#define UC_CORE_OBDREQUEST_H

#include "ObdPidValue.h"

namespace core { namespace can { namespace obd {
    class ObdRequest {
    public:
        ObdRequest() {}
        ObdRequest& add(uint8_t pid, uint8_t length) {
            add(ObdPidValue::ObdPidValue(pid, length));
            return *this;
        }

        void add(ObdPidValue value) {
            if (_pidCount < 6)
                _pid[_pidCount++] = value;

        }

        void update(ObdPidValue value) {
            for (size_t i = 0; i < _pidCount; i++) {
                if (_pid[i].Pid == value.Pid) {
                    _pid[i] = value;
                    return;
                }
            }
        }

        ObdPidValue getByPid(uint8_t pid) const {
            for (size_t i = 0; i < _pidCount; i++)
                if (_pid[i].Pid == pid)
                    return _pid[i];

            return InvalidPid;
        }

        ObdPidValue at(uint8_t index) {
            if (index < _pidCount)
                return _pid[index];
            return InvalidPid;
        }

        uint8_t count() {
            return _pidCount;
        }
    private:
        ObdPidValue _pid[6] = {0, 0, 0, 0, 0, 0};
        uint8_t _pidCount = 0;
    };
}}}

#endif //UC_CORE_OBDREQUEST_H
