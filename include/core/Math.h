//
// Created by tyll on 07.02.22.
//

#ifndef UC_CORE_MATH_H
#define UC_CORE_MATH_H
#include <stdint.h>
#include <math.h>

// Function definitions are guarded to avoid clashes with their Arduino counterparts if the Arduino lib is used.
#ifndef min
        template<typename T>
        T min(T val0, T val1) {
            return val0 < val1 ? val0 : val1;
        }
#endif
#ifndef max
        template<typename T>
        T max(T val0, T val1) {
            return val0 > val1 ? val0 : val1;
        }
#endif
#ifndef constrain
        template<typename T>
        T constrain(T val, T lowerLimit, T upperLimit) {
            return min(upperLimit, max(lowerLimit, val));
        }
#endif
#ifndef sq
        template<typename T>
        T sq(T val) {
            return val * val;
        }
#endif
#ifndef abs
        template<typename T>
        T abs(T val) {
            return ((val) > 0 ? (val) : -(val));
        }
#endif

inline float mapF(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#endif //UC_CORE_MATH_H
