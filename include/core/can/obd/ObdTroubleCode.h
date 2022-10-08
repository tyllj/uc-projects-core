//
// Created by tyll on 2022-10-07.
//

#ifndef UC_CORE_OBDTROUBLECODE_H
#define UC_CORE_OBDTROUBLECODE_H

#include <stdint.h>
#include "core/CStrings.h"
#include "core/StringBuilder.h"
#include "core/Bits.h"
namespace core { namespace can { namespace obd {
    struct ObdTroubleCode {
    public:
        ObdTroubleCode() : A(0), B(0) {}
        ObdTroubleCode(uint8_t a, uint8_t b) : A(a), B(b) {}
        uint8_t A;
        uint8_t B;

        void toString(char* str, size_t n) {
            core::StringBuilder sb(str, n);
            sb.append((A >> 6) == 0 ? 'P' : (A >> 6) == 1 ? 'C' : (A >> 6) == 2 ? 'B' : 'U');
            sb.appendHex(static_cast<uint8_t>((A >> 6) & 0x03));
            sb.appendHex(static_cast<uint8_t>(A & 0x0F));
            sb.appendHex(static_cast<uint8_t>((B >> 4) & 0x0F));
            sb.appendHex(static_cast<uint8_t>(B & 0x0F));
        };

        template<size_t n>
        void toString(char (&str)[n]) {
            toString(str, n);
        }

        core::CString toString() {
            core::CString str(6);
            toString(str.get(), 6);
            return str;
        }

    };
}}}

#endif //UC_CORE_OBDTROUBLECODE_H
