//
// Created by tyll on 21.09.22.
//

#ifndef UC_CORE_ELMRUNTIMECONFIGURATION_H
#define UC_CORE_ELMRUNTIMECONFIGURATION_H

#include <stdint.h>
#include "core/can/ICanInterface.h"

namespace core { namespace can {namespace elm {
    constexpr char ELM_VERSION_STRING[] = "ELM327 v1.4";
    constexpr char ELM_PROGRAMMABLE_PARAMETERS[] =
            "00:FF F  01:FF F  02:FF F  03:32 F\r"
            "04:01 F  05:FF F  06:F1 F  07:09 F\r"
            "08:FF F  09:00 F  0A:0A F  0B:FF F\r"
            "0C:68 F  0D:0D F  0E:9A F  0F:FF F\r"
            "10:0D F  11:00 F  12:FF F  13:32 F\r"
            "14:FF F  15:0A F  16:FF F  17:92 F\r"
            "18:00 F  19:28 F  1A:FF F  1B:FF F\r"
            "1C:FF F  1D:FF F  1E:FF F  1F:FF F\r"
            "20:FF F  21:FF F  22:FF F  23:FF F\r"
            "24:00 F  25:00 F  26:00 F  27:FF F\r"
            "28:FF F  29:FF F  2A:38 N  2B:02 F\r"
            "2C:81 N  2D:04 N  2E:81 N  2F:01 N";

    struct ElmRuntimeConfiguration {
        canid_t filterId = 0x7E8;
        canid_t maskId = 0x7F0;
        canid_t transmitAddress = 0x7DF;
        uint8_t timeoutMultiplier = 0x32;
        uint8_t testerAddress = 0xF9;
        uint8_t adaptiveTimingMode = 1;
        uint8_t protocol = 6;
        bool bypassInit = false;
        bool isEchoOn = true;
        bool isHeadersOn = true;
        bool isResponsesOn = false;
        bool isSpacesOn = true;
        bool isLineFeedsOn = true;
        bool isAutoFormattingOn = true;
    };
}}}
#endif //UC_CORE_ELMRUNTIMECONFIGURATION_H
