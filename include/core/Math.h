//
// Created by tyll on 07.02.22.
//

#ifndef SGLOGGER_MATH_H
#define SGLOGGER_MATH_H
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
#endif //SGLOGGER_MATH_H
