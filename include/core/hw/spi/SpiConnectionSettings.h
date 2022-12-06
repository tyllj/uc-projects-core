//
// Created by tu.jungke on 10.11.2022.
//

#ifndef __SPICONNECTIONSETTINGS_HPP__
#define __SPICONNECTIONSETTINGS_HPP__

#include <stdint.h>

namespace core { namespace hw { namespace spi {
    enum class SpiMode : uint8_t {
        Cpol0Cpha0 = 0, // Polarity is idled low and data is sampled on rising edge of the clock signal.
        Cpol0Cpha1 = 1, // Polarity is idled low and data is sampled on falling edge of the clock signal.
        Cpol1Cpha0 = 2, // Polarity is idled high and data is sampled on falling edge of the clock signal.
        Cpol1Cpha1 = 3, // Polarity is idled high and data is sampled on rising edge of the clock signal.
        Mode0 = Cpol0Cpha0,
        Mode1 = Cpol0Cpha1,
        Mode2 = Cpol1Cpha0,
        Mode3 = Cpol1Cpha1
    };

    enum class DataFlow : uint8_t {
        MsbFirst = 0,
        LsbFirst = 1
    };

    struct SpiConnectionSettings {
        uint32_t ClockFrequency;
        uint8_t ChipSelectLine;
        uint8_t ChipSelectLineActiveState;
        uint8_t DataBitLength;
        DataFlow DataFlow;
        SpiMode Mode;
    };
}}}
#endif //__SPICONNECTIONSETTINGS_HPP__
