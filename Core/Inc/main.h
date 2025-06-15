/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_R_Pin LL_GPIO_PIN_0
#define LED_R_GPIO_Port GPIOC
#define LED_B_Pin LL_GPIO_PIN_1
#define LED_B_GPIO_Port GPIOC
#define B7_Pin LL_GPIO_PIN_8
#define B7_GPIO_Port GPIOE
#define B6_Pin LL_GPIO_PIN_9
#define B6_GPIO_Port GPIOE
#define B5_Pin LL_GPIO_PIN_10
#define B5_GPIO_Port GPIOE
#define B4_Pin LL_GPIO_PIN_11
#define B4_GPIO_Port GPIOE
#define B3_Pin LL_GPIO_PIN_12
#define B3_GPIO_Port GPIOE
#define B2_Pin LL_GPIO_PIN_13
#define B2_GPIO_Port GPIOE
#define B1_Pin LL_GPIO_PIN_14
#define B1_GPIO_Port GPIOE
#define B0_Pin LL_GPIO_PIN_15
#define B0_GPIO_Port GPIOE
#define test_IO_Pin LL_GPIO_PIN_8
#define test_IO_GPIO_Port GPIOA
#define KEY0_Pin LL_GPIO_PIN_0
#define KEY0_GPIO_Port GPIOD
#define KEY1_Pin LL_GPIO_PIN_1
#define KEY1_GPIO_Port GPIOD
#define KEY2_Pin LL_GPIO_PIN_2
#define KEY2_GPIO_Port GPIOD
#define KEY3_Pin LL_GPIO_PIN_3
#define KEY3_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#define 	and_uart_buf_size		512
#define 	pc_uart_buf_size		512

#define 	KEY0_READ()					LL_GPIO_IsInputPinSet(KEY0_GPIO_Port,KEY0_Pin)
#define 	KEY1_READ()					LL_GPIO_IsInputPinSet(KEY1_GPIO_Port,KEY1_Pin)
#define 	KEY2_READ()					LL_GPIO_IsInputPinSet(KEY2_GPIO_Port,KEY2_Pin)
#define 	KEY3_READ()					LL_GPIO_IsInputPinSet(KEY3_GPIO_Port,KEY3_Pin)

#define 	DAT_SET()						LL_GPIO_SetOutputPin(B0_GPIO_Port,B0_Pin);
#define 	DAT_RESET()					LL_GPIO_ResetOutputPin(B0_GPIO_Port,B0_Pin);
#define 	CLK_SET()						LL_GPIO_SetOutputPin(B0_GPIO_Port,B1_Pin);
#define 	CLK_RESET()					LL_GPIO_ResetOutputPin(B0_GPIO_Port,B1_Pin);
#define 	SYNC1_SET()					LL_GPIO_SetOutputPin(B0_GPIO_Port,B3_Pin);
#define 	SYNC1_RESET()				LL_GPIO_ResetOutputPin(B0_GPIO_Port,B3_Pin);
#define 	SYNC2_SET()					LL_GPIO_SetOutputPin(B0_GPIO_Port,B2_Pin);
#define 	SYNC2_RESET()				LL_GPIO_ResetOutputPin(B0_GPIO_Port,B2_Pin);


extern volatile uint8_t 	time3_1000ms_flag,time3_500ms_flag,time3_100ms_flag,time3_50ms_flag,time3_10ms_flag,time3_1ms_flag;
extern volatile uint8_t 	and_uart_stop_cnt,and_rx_buffer[and_uart_buf_size],and_rx_buffer_bk[and_uart_buf_size];
extern volatile uint8_t 	pc_uart_stop_cnt,pc_rx_buffer[pc_uart_buf_size],pc_rx_buffer_bk[pc_uart_buf_size];
extern volatile uint32_t 	and_current_len_bk,and_current_len,pc_current_len,pc_current_len_bk;
void pc_uart_send( uint8_t* dat,uint32_t len);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
