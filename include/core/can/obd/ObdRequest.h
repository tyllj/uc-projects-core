//
// Created by tyll on 2022-10-01.
//

#ifndef UC_CORE_OBDREQUEST_H
#define UC_CORE_OBDREQUEST_H

#include "ObdValue.h"

namespace core { namespace can { namespace obd {
    class ObdRequest {
    public:
        ObdRequest() {}
        ObdRequest& add(uint8_t pid, uint8_t length) {
            add(ObdValue::empty(pid, length));
            return *this;
        }

        void add(ObdValue value) {
            if (_pidCount < 6)
                _pid[_pidCount++] = value;

        }

        void update(ObdValue value) {
            for (size_t i = 0; i < _pidCount; i++) {
                if (_pid[i].Pid == value.Pid) {
                    _pid[i] = value;
                    return;
                }
            }
        }

        ObdValue getByPid(uint8_t pid) {
            for (size_t i = 0; i < _pidCount; i++)
                if (_pid[i].Pid == pid)
                    return _pid[i];

            return InvalidPid;
        }

        ObdValue at(uint8_t index) {
            if (index < _pidCount)
                return _pid[index];
            return InvalidPid;
        }

        uint8_t count() {
            return _pidCount;
        }
    private:
        ObdValue _pid[6] = {0, 0, 0, 0, 0, 0};
        uint8_t _pidCount = 0;
    };
}}}

#endif //UC_CORE_OBDREQUEST_H
