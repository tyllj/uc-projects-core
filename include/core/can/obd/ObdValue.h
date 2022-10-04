//
// Created by tyll on 2022-10-01.
//

#ifndef UC_CORE_OBDVALUE_H
#define UC_CORE_OBDVALUE_H

#include <stdint.h>

namespace core { namespace can { namespace obd {


    struct ObdValue {
    public:
        uint8_t A = 0;
        uint8_t B = 0;
        uint8_t C = 0;
        uint8_t D = 0;
        uint8_t Pid = 0xFF;
        uint8_t DataLength = 0;
    public:
        ObdValue(uint8_t pid) : A(0), B(0), C(0), D(0), Pid(pid), DataLength(0) {}
        ObdValue(uint8_t pid, uint8_t a) : A(a), B(0), C(0), D(0), Pid(pid), DataLength(1) {}
        ObdValue(uint8_t pid, uint8_t a, uint8_t b) : A(a), B(b), C(0), D(0), Pid(pid), DataLength(2) {}
        ObdValue(uint8_t pid, uint8_t a, uint8_t b, uint8_t c) : A(a), B(b), C(c), D(0), Pid(pid), DataLength(3) {}
        ObdValue(uint8_t pid, uint8_t a, uint8_t b, uint8_t c, uint8_t d) : A(a), B(b), C(c), D(d), Pid(pid), DataLength(4) {}

        static ObdValue empty(uint8_t pid, uint8_t expectedLength) {
            return ObdValue(pid, nullptr, expectedLength);
        }

        ObdValue(uint8_t pid, const uint8_t* data, uint8_t length) : A(0), B(0), C(0), D(0), Pid(pid), DataLength(min(length,4)) {
            if (data != nullptr) {
                switch (DataLength) {
                    case 4: D = data[3]; [[fallthrough]];
                    case 3: C = data[2]; [[fallthrough]];
                    case 2: B = data[1]; [[fallthrough]];
                    case 1: A = data[0]; [[fallthrough]];
                    default:
                        break;
                }
            }
        }

        uint8_t asUint8() { return A; }
        uint16_t asUint16() { return (static_cast<uint16_t>(A) << 8) | static_cast<uint16_t>(B); }
        uint32_t asUint32() { return (static_cast<uint16_t>(A) << 24) | (static_cast<uint16_t>(B) << 16) | (static_cast<uint16_t>(C) << 8) |
                                     static_cast<uint16_t>(D); }
        uint8_t& operator[](uint8_t i) {
            switch (i % 4) {
                case 0: return A;
                case 1: return B;
                case 2: return C;
                case 3: return D;
                default: return A;
            }
        };

    };

    static inline const ObdValue InvalidPid(0xFF);

    bool isInvalidPid(ObdValue value) { return value.Pid == 0xFF; }
}}}

#endif //UC_CORE_OBDVALUE_H
