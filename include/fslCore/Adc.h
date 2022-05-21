/*
 * Adc.h
 *
 *  Created on: May 15, 2022
 *      Author: tyll
 */

#ifndef HAL_ADC_H_
#define HAL_ADC_H_

#include <fsl_adc16.h>

namespace fslCore {
    class Adc {
    public:
        Adc(ADC_Type *base) : _base(base) {
            ADC16_GetDefaultConfig(&_config);
            ADC16_Init(base, &_config);
            ADC16_EnableHardwareTrigger(base, false);
#if defined(FSL_FEATURE_ADC16_HAS_CALIBRATION) && FSL_FEATURE_ADC16_HAS_CALIBRATION
            ADC16_DoAutoCalibration(base);
#endif
        }

        virtual ~Adc() {
            ADC16_Deinit(_base);
        }

        ADC_Type *getBase() {
            return _base;
        }

    private:
        ADC_Type *_base;
        adc16_config_t _config;
    };

    class AnalogInput {
    public:
        AnalogInput(Adc &adc, uint32_t channelGroup, uint32_t channel) :
                _base(adc.getBase()),
                _channelGroup(channelGroup),
                _channel(channel) {
        }

        uint32_t analogRead() {
            _config.channelNumber = _channel;
            _config.enableInterruptOnConversionCompleted = false;
#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
            _config.enableDifferentialConversion = false;
#endif
            ADC16_SetChannelConfig(_base, _channelGroup, &_config);
            while (!(kADC16_ChannelConversionDoneFlag &
                     ADC16_GetChannelStatusFlags(_base, _channelGroup)));
            return ADC16_GetChannelConversionValue(_base, _channelGroup);
        }

        virtual ~AnalogInput() {

        }

    private:
        ADC_Type *_base;
        adc16_channel_config_t _config;
        uint32_t _channelGroup;
        uint32_t _channel;
    };
}

#endif /* HAL_ADC_H_ */
