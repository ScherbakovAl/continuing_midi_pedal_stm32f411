/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "rtc.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "power.h"
#include <deque>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
using uint = unsigned int;
using cuint = const uint;
cuint extpr0 = 1;
cuint extpr1 = 2;
cuint extpr2 = 4;
cuint extpr3 = 8;
int32_t t = 0;
int32_t p = 0;
int f = 0;
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	if (f == 1 && hadc->Instance == ADC1) {
		t = HAL_ADC_GetValue(&hadc1);
		if (t < 300) {
			t = 300;
		}
		if (t - p > 14 || p - t > 14) {
			MidiSender((uint32_t) t);
			p = t;
		}
	}
}

enum class pedal {
	a = EXTI_IMR_MR0, b = EXTI_IMR_MR1, c = EXTI_IMR_MR2, d = EXTI_IMR_MR3
};

class pedals {
public:
	pedal ped = pedal::a;
	uint32_t time = 0;
};

std::deque<pedals> dequePedals;

//using uint = unsigned int;
//using cuint = const uint;

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_ADC1_Init();
	MX_TIM3_Init();
	MX_TIM2_Init();
	MX_RTC_Init();
	MX_TIM5_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	HAL_ADC_Start_IT(&hadc1);
	HAL_TIM_Base_Start(&htim3);
	HAL_Delay(15);
	pwr();
	HAL_TIM_Base_Start(&htim5);
	MX_USB_DEVICE_Init();
	HAL_TIM_Base_Start_IT(&htim2);
	f = 1;
	GPIOC->BSRR = 0x20000000;

	while (1) {
		auto timer = TIM5->CNT;
		if (!dequePedals.empty()) {
			auto &dP = dequePedals.front();
			if (timer - dP.time > 4200) {
				EXTI->IMR |= (uint32_t) dP.ped;
				dequePedals.pop_front();
				GPIOC->BSRR = 0x20000000;
			}
		}
		if (timer > 4'094'967'295) {
			TIM5->CNT = 0;
		}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI
			| RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 12;
	RCC_OscInitStruct.PLL.PLLN = 96;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
extern "C" {
void EXTI0_IRQHandler(void) {
	EXTI->PR = extpr0;
	EXTI->IMR &= ~(EXTI_IMR_MR0);
	dequePedals.push_back( { pedal::a, TIM5->CNT });
	MidiSender2(1, 50, 11);
	GPIOC->BSRR = 0x2000;
}

void EXTI1_IRQHandler(void) {
	EXTI->PR = extpr1;
	EXTI->IMR &= ~(EXTI_IMR_MR1);
	dequePedals.push_back( { pedal::b, TIM5->CNT });
	MidiSender2(1, 51, 22);
	GPIOC->BSRR = 0x2000;
}

void EXTI2_IRQHandler(void) {
	EXTI->PR = extpr2;
	EXTI->IMR &= ~(EXTI_IMR_MR2);
	dequePedals.push_back( { pedal::c, TIM5->CNT });
	MidiSender2(2, 60, 33);
	GPIOC->BSRR = 0x2000;
}

void EXTI3_IRQHandler(void) {
	EXTI->PR = extpr3;
	EXTI->IMR &= ~(EXTI_IMR_MR3);
	dequePedals.push_back( { pedal::d, TIM5->CNT });
	MidiSender2(2, 61, 44);
	GPIOC->BSRR = 0x2000;
}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
