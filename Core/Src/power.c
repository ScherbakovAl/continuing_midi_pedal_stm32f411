/*
 * power.c
 *
 *  Created on: May 16, 2024
 *      Author: sche
 */
#include "stm32f4xx_hal.h"
extern ADC_HandleTypeDef hadc1;

void pwr() {

	if (!__HAL_PWR_GET_FLAG(PWR_FLAG_SB)) {
		HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
		HAL_PWR_EnterSTANDBYMode();
	} else {
		HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
		int val = HAL_ADC_GetValue(&hadc1);
		if (val > 2000) {
		} else {
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
			HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
			HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
			HAL_PWR_EnterSTANDBYMode();
		}
	}
}
