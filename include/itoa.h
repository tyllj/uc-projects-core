//
// Created by tyll on 11.02.22.
//

#ifndef FIRMWARE_ITOA_H
#define FIRMWARE_ITOA_H

// Conversion functions which are not part of ISO-CPP. The AVR-libc implements these in avr assembly language.
// For all other platforms pure C++ versions are provided here.

#ifdef __AVR__
// Those functions are already defined in the AVR libc.
#include <stdlib.h>
#else
#include <string.h>
namespace {
    inline void reverse(char* begin, char* end) {
        char *is = begin;
        char *ie = end - 1;
        while(is < ie) {
            char tmp = *ie;
            *ie = *is;
            *is = tmp;
            ++is;
            --ie;
        }
    }

    inline char* ltoa(long value, char* result, int base) {
        if(base < 2 || base > 16) {
            *result = 0;
            return result;
        }

        char* out = result;
        long quotient = abs(value);

        do {
            const long tmp = quotient / base;
            *out = "0123456789abcdef"[quotient - (tmp * base)];
            ++out;
            quotient = tmp;
        } while(quotient);

        // Apply negative sign
        if(value < 0)
            *out++ = '-';

        reverse(result, out);
        *out = 0;
        return result;
    }

    inline char* ultoa(unsigned long value, char* result, int base) {
        if(base < 2 || base > 16) {
            *result = 0;
            return result;
        }

        char* out = result;
        unsigned long quotient = value;

        do {
            const unsigned long tmp = quotient / base;
            *out = "0123456789abcdef"[quotient - (tmp * base)];
            ++out;
            quotient = tmp;
        } while(quotient);

        reverse(result, out);
        *out = 0;
        return result;
    }

    inline char *itoa(int value, char *string, int radix) {
        return ltoa(value, string, radix);
    }

    inline char *utoa(unsigned long value, char *string, int radix) {
        return ultoa(value, string, radix);
    }

    inline char * dtostrf(double number, signed int width, unsigned int prec, char *s) {
        bool negative = false;

        if (isnan(number)) {
            strcpy(s, "nan");
            return s;
        }
        if (isinf(number)) {
            strcpy(s, "inf");
            return s;
        }

        char* out = s;

        int fillme = width; // how many cells to fill for the integer part
        if (prec > 0) {
            fillme -= (prec+1);
        }

        // Handle negative numbers
        if (number < 0.0) {
            negative = true;
            fillme--;
            number = -number;
        }

        // Round correctly so that print(1.999, 2) prints as "2.00"
        // I optimized out most of the divisions
        double rounding = 2.0;
        for (uint32_t i = 0; i < prec; ++i)
            rounding *= 10.0;
        rounding = 1.0 / rounding;

        number += rounding;

        // Figure out how big our number really is
        double tenpow = 1.0;
        int digitcount = 1;
        while (number >= 10.0 * tenpow) {
            tenpow *= 10.0;
            digitcount++;
        }

        number /= tenpow;
        fillme -= digitcount;

        // Pad unused cells with spaces
        while (fillme-- > 0) {
            *out++ = ' ';
        }

        // Handle negative sign
        if (negative) *out++ = '-';

        // Print the digits, and if necessary, the decimal point
        digitcount += prec;
        int8_t digit = 0;
        while (digitcount-- > 0) {
            digit = (int8_t)number;
            if (digit > 9) digit = 9; // insurance
            *out++ = (char)('0' | digit);
            if ((digitcount == prec) && (prec > 0)) {
                *out++ = '.';
            }
            number -= digit;
            number *= 10.0;
        }

        // make sure the string is terminated
        *out = 0;
        return s;
    }
}
#endif //__AVR__
#endif //FIRMWARE_ITOA_H
