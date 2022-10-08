/*
 * PwmOutput.h
 *
 *  Created on: Jul 5, 2022
 *      Author: tyll
 */

#ifndef HAL_PWMOUTPUT_H_
#define HAL_PWMOUTPUT_H_

#include <fsl_tpm.h>

namespace fslCore {
	class PwmOutput {
	public:
		PwmOutput(TPM_Type* pwm, tpm_chnl_t channel, tpm_pwm_level_select_t level = kTPM_HighTrue) {
			tpm_config_t tpmConfig;
			TPM_GetDefaultConfig(&tpmConfig);

			tpm_chnl_pwm_signal_param_t tpmParam;

			tpmParam.chnlNumber = channel;
			tpmParam.level = kTPM_NoPwmSignal;

			TPM_Init(pwm, &tpmConfig);
			TPM_SetupPwm(pwm, &tpmParam, 1U, kTPM_CenterAlignedPwm, 24000U, CLOCK_GetFreq(kCLOCK_PllFllSelClk));
			TPM_StartTimer(pwm, kTPM_SystemClock);
		}

		~PwmOutput() {
			TPM_UpdateChnlEdgeLevelSelect(_pwm, _ch, kTPM_NoPwmSignal);
		}

		void analogWrite(uint8_t value) {
	        /* Disable channel output before updating the dutycycle */
			uint16_t percent = value == 255 ? 100 : (value * 100) / 256; // rescale, as the NXP driver uses percentage values
	        TPM_UpdateChnlEdgeLevelSelect(_pwm, _ch, kTPM_NoPwmSignal);

	        /* Update PWM duty cycle */
	        TPM_UpdatePwmDutycycle(_pwm, _ch, kTPM_CenterAlignedPwm, (uint8_t) percent);

	        /* Start channel output with updated dutycycle */
	        TPM_UpdateChnlEdgeLevelSelect(_pwm, _ch, _level);
		}
	private:
		TPM_Type* _pwm;
		tpm_chnl_t _ch;
		tpm_pwm_level_select_t _level;
	};
}


#endif /* HAL_PWMOUTPUT_H_ */
