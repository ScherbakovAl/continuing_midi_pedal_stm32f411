/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : usb_device.c
 * @version        : v1.0_Cube
 * @brief          : This file implements the USB Device
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

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */
USBD_CDC_HandleTypeDef *hcdcdc;
uint8_t u = 0;
uint8_t r = 0;
uint8_t buf[4];

void MidiSender(uint32_t c) {
	u = c / 30 - 9;
	if(r != u){
		if (hcdcdc->TxState == 0) { //(0==свободно, !0==занято)
			buf[0] = 9;
			buf[1] = 176;
			buf[2] = 64;
			buf[3] = u;

			USBD_CDC_SetTxBuffer(&hUsbDeviceFS, buf, 4);
			USBD_CDC_TransmitPacket(&hUsbDeviceFS);
			r = u;
			TIM2->CNT = 0;
		}
	}
}
/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
 * Init USB device Library, add supported class and start the library
 * @retval None
 */
void MX_USB_DEVICE_Init(void) {
	/* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */

	/* USER CODE END USB_DEVICE_Init_PreTreatment */

	/* Init Device Library, add supported class and start the library. */
	if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK) {
		Error_Handler();
	}
	if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK) {
		Error_Handler();
	}
	if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS)
			!= USBD_OK) {
		Error_Handler();
	}
	if (USBD_Start(&hUsbDeviceFS) != USBD_OK) {
		Error_Handler();
	}

	/* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */

	uint8_t txbuf[8]; //start midi mini test
	//on
	txbuf[0] = 9; //??
	txbuf[1] = 144; //0x90(144) - note on, 0x80(128) - note off //0xB0 - Control Change (for hi-res midi) | channel
	txbuf[2] = 55; //note	// 88 (for hi-res midi)
	txbuf[3] = 60; //vel	// velocity xx.75 (for hi-res midi)

	//off
	txbuf[4] = 9; //0x09	//??
	txbuf[5] = 128; //0x90(144) - note on, 0x80(128) - note off | channel
	txbuf[6] = 55; //note
	txbuf[7] = 120; //vel	//velocity 86.xx (for hi-res midi)

	HAL_Delay(800);

	CDC_Transmit_FS(txbuf, 8);
	HAL_Delay(100);
	CDC_Transmit_FS(txbuf, 8);

	hcdcdc = (USBD_CDC_HandleTypeDef*) hUsbDeviceFS.pClassData; // свободно для отправки? "if (hcdcdc->TxState == ...)"; (0==свободно, !0==занято)
	/* USER CODE END USB_DEVICE_Init_PostTreatment */
}

/**
 * @}
 */

/**
 * @}
 */

