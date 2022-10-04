//
// Created by tyll on 2022-10-01.
//

#ifndef UC_CORE_PIDDEFINITIONS_H
#define UC_CORE_PIDDEFINITIONS_H

#include <cstdint>

namespace core { namespace can { namespace obd {
    constexpr uint8_t getDataLengthForPid(uint8_t pid) {
        return 2;
    }
}}}

#endif //UC_CORE_PIDDEFINITIONS_H
