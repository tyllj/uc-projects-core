//
// Created by tyll on 07.02.22.
//

#ifndef UC_CORE_MATH_H
#define UC_CORE_MATH_H
#include <stdint.h>
#include <math.h>

#ifdef DEG_TO_RAD
#define DEG_TO_RAD  0.017453292519943295769236907684886
#endif

#ifdef abs
#undef abs
#endif
template<typename T>
inline T abs(T x) {
    return x > 0 ? x : -x;
}

#ifdef sq
#undef sq
#endif
template<typename T>
inline T sq(T x) {
    return x * x;
}

#ifdef min
#undef min
#endif
template<typename T, typename U>
inline T min(T a, U b) {
    return static_cast<T>(a < b ? a : b);
}

#ifdef max
#undef max
#endif
template<typename T, typename U>
inline T max(T a, U b) {
    return static_cast<T>(a > b ? a : b);
}

#ifdef round
#undef round
#endif
template<typename T>
inline int32_t round(T x){
   return x >= 0 ? (long)(x + 0.5) : (long)(x - 0.5);
}

#ifdef radians
#undef radians
#endif
#define radians(deg) ((deg) * DEG_TO_RAD)

#ifdef degrees
#undef degrees
#endif
#define degrees(rad) ((rad) * RAD_TO_DEG)

#ifdef constrain
#undef constrain
#endif
template<typename T, typename U, typename V>
inline V constrain(T x, U low, V high) {
    return x < low ? low : x > high ? high : x;
}

inline float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline uint32_t mapInt(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#endif //UC_CORE_MATH_H
