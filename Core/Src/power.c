/*
 * power.c
 *
 *  Created on: May 16, 2024
 *      Author: sche
 */
extern ADC_HandleTypeDef hadc1;
pwr(){

	if (!__HAL_PWR_GET_FLAG(PWR_FLAG_SB)) {
		ssd1306_Fill(Black);
		ssd1306_SetCursor(10, 20);
		ssd1306_WriteString("STANDBY..1", Font_16x15, White);
		ssd1306_UpdateScreen();
		HAL_Delay(200);

		HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1); // A0 к земле. +5 включает плату
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
		HAL_PWR_EnterSTANDBYMode();

	} else {
		HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

		int val = HAL_ADC_GetValue(&hadc1);

		if (val < 2400) {
			ssd1306_Fill(Black);
			ssd1306_SetCursor(10, 20);
			ssd1306_WriteString("START", Font_16x15, White);
			ssd1306_UpdateScreen();
			HAL_Delay(200);
		} else {
//			__HAL_RTC_ALARM
//			RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
			ssd1306_Fill(Black);
			ssd1306_SetCursor(10, 20);
			ssd1306_WriteString("STANDBY..2", Font_16x15, White);
			ssd1306_UpdateScreen();
			HAL_Delay(200);


			HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
			HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
			HAL_PWR_EnterSTANDBYMode();
		}
	}
}
