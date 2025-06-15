/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v1.0_Cube
  * @brief           : This file implements the USB Host
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_audio.h"
#include "usbh_cdc.h"
#include "usbh_msc.h"
#include "usbh_hid.h"
#include "usbh_mtp.h"
#include "usbh_ft232.h"
#include <string.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostFS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */
USBH_StatusTypeDef usbresult;
/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */

  /* USER CODE END USB_HOST_Init_PreTreatment */

  /* Init host Library */
  if (USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS) != USBH_OK)
  {
    Error_Handler();
  }
  
  /* 注册FT232类 */
  if (USBH_RegisterClass(&hUsbHostFS, USBH_FT232_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  
  if (USBH_Start(&hUsbHostFS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */

  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * Background task
 */
void MX_USB_HOST_Process(void)
{
  /* USB Host Background task */
  USBH_Process(&hUsbHostFS);
}

/*
 * user callback definition
 */

static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
  switch(id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
	  USBH_UsrLog("HOST_USER_SELECT_CONFIGURATION-------------------------");
    break;
  case HOST_USER_CLASS_ACTIVE:
    Appli_state = APPLICATION_READY;
    USBH_UsrLog("APPLICATION_READY-------------------------");
    
    /* FT232设备已就绪，可以进行串口通信 */
    if(phost->pActiveClass != NULL && 
       strcmp(phost->pActiveClass->Name, "FT232") == 0)
    {
      USBH_UsrLog("FT232 device ready for communication!");
      USBH_UsrLog("Configuration: 9600 baud, 7 data bits, 1 stop bit, even parity");
      
      /* 示例：发送测试数据 */
      uint8_t test_data[] = "Hello FT232!\r\n";
      USBH_StatusTypeDef result = USBH_FT232_Transmit(phost, test_data, sizeof(test_data)-1);
      if(result == USBH_OK)
      {
        USBH_UsrLog("Test data sent successfully");
      }
      else
      {
        USBH_UsrLog("Failed to send test data");
      }
    }
    break;
  case HOST_USER_CLASS_SELECTED:
	  USBH_UsrLog("HOST_USER_CLASS_SELECTED-------------------------");
    break;
  case HOST_USER_CONNECTION:
    Appli_state = APPLICATION_START;
    USBH_UsrLog("APPLICATION_START-------------------------");
    break;
  case HOST_USER_DISCONNECTION:
    Appli_state = APPLICATION_IDLE;
    USBH_UsrLog("APPLICATION_IDLE-------------------------");
    break;
  case HOST_USER_UNRECOVERED_ERROR:
    Appli_state = APPLICATION_DISCONNECT;
    USBH_UsrLog("APPLICATION_DISCONNECT-------------------------");
    break;
  default:
	  USBH_UsrLog("other-------------------------");
    break;
  }
}

/**
  * @}
  */

/**
  * @}
  */

