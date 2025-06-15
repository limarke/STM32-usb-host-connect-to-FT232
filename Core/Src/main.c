/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbh_cdc.h"
#include <stdio.h>
#include "usbh_core.h"
#include "usbh_ft232.h"
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
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
#include "crc_xmod.h"
volatile uint8_t 		time3_1000ms_flag=0,time3_500ms_flag=0,time3_100ms_flag=0,time3_50ms_flag=0,time3_10ms_flag=0,time3_1ms_flag ;
volatile uint8_t 		and_uart_stop_cnt=0,and_rx_buffer[and_uart_buf_size],and_rx_buffer_bk[and_uart_buf_size];
volatile uint8_t 		pc_uart_stop_cnt=0,pc_rx_buffer[pc_uart_buf_size],pc_rx_buffer_bk[pc_uart_buf_size];
volatile uint32_t 		and_current_len_bk=0,and_current_len=0,pc_current_len=0,pc_current_len_bk=0;
volatile uint8_t 		temp1,temp2;
volatile uint16_t		crc_temp1;
volatile uint8_t    	send_buf[64],line_coding_set=0;
volatile uint32_t 		voltage1=0,voltage2=0;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define COMPILER_BARRIER() __asm volatile("" ::: "memory")
uint8_t flag;

extern USBH_HandleTypeDef hUsbHostFS;
extern ApplicationTypeDef Appli_state;
extern USBH_StatusTypeDef usbresult;

#define RX_BUFF_SIZE   64  /* Max Received data 1KB */

uint8_t CDC_RX_Buffer[RX_BUFF_SIZE];
uint8_t CDC_TX_Buffer[RX_BUFF_SIZE];

typedef enum {
  CDC_STATE_IDLE = 0,
  CDC_SEND,
  CDC_RECEIVE,
}CDC_StateTypedef;

CDC_StateTypedef CDC_STATE = CDC_STATE_IDLE;

uint8_t i=0;
void CDC_HANDLE (void)
{
	switch (CDC_STATE)
	{
	case CDC_STATE_IDLE:
	{
		  USBH_CDC_Stop(&hUsbHostFS);
//
		  CDC_TX_Buffer[0]=0x02;
		  CDC_TX_Buffer[1]=0x30;
		  CDC_TX_Buffer[2]=0x30;
		  CDC_TX_Buffer[3]=0x35;
		  CDC_TX_Buffer[4]=0x34;
		  CDC_TX_Buffer[5]=0x31;
		  CDC_TX_Buffer[6]=0x20;
		  CDC_TX_Buffer[7]=0x20;
		  CDC_TX_Buffer[8]=0x20;
		  CDC_TX_Buffer[9]=0x03;
		  CDC_TX_Buffer[10]=0x31;
		  CDC_TX_Buffer[11]=0x33;
		  CDC_TX_Buffer[12]=0x0D;
		  CDC_TX_Buffer[13]=0x0A;
		  if (USBH_CDC_Transmit (&hUsbHostFS, CDC_TX_Buffer, 14) == USBH_OK)
		  {
			  CDC_STATE = CDC_RECEIVE;
		  }

		  i++;
		  break;
	}

	case CDC_RECEIVE:
	{
		  USBH_CDC_Stop(&hUsbHostFS);
		  usbresult = USBH_CDC_Receive(&hUsbHostFS, (uint8_t *) CDC_RX_Buffer, RX_BUFF_SIZE);
		  HAL_Delay (1000);
		  pc_uart_send(CDC_RX_Buffer,RX_BUFF_SIZE);
		  CDC_STATE = CDC_IDLE;
	}

	default:
		  break;
	}
}

void my_copy(uint8_t*desti,uint8_t* sourc,uint32_t len)
{
	for(uint32_t i=0;i<len;i++){	desti[i]=sourc[i];}
}
void delay_us(uint32_t cnt)
{
	while(cnt--)
	{
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	}
}
#define delay_100ns() {__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();}
void delay_ms(uint32_t cnt)
{
	while(cnt--)
	{
		delay_us(1000);
	}
}
#define  dac_delay()  delay_us(1)
//#define  dac_delay()  delay_100ns()
void set_voltage1(uint32_t valuee)
{
	SYNC1_SET();
	CLK_SET()
	dac_delay();
	SYNC1_RESET();
	dac_delay();
	valuee<<=8;
	for(uint32_t i=0;i<24;i++)
	{
		if(valuee&0x80000000)	{DAT_SET();}
		else									{DAT_RESET();}
		valuee<<=1;
		dac_delay();
		CLK_RESET();
		dac_delay();
		CLK_SET();
	}
	SYNC1_SET();
}
void set_voltage2(uint32_t valuee)
{
	SYNC2_SET();
	CLK_SET()
	dac_delay();
	SYNC2_RESET();
	dac_delay();
	valuee<<=8;
	for(uint32_t i=0;i<24;i++)
	{
		if(valuee&0x80000000)	{DAT_SET();}
		else									{DAT_RESET();}
		valuee<<=1;
		dac_delay();
		CLK_RESET();
		dac_delay();
		CLK_SET();
	}
	SYNC2_SET();
}
void send_data_1(USART_TypeDef *USARTx, uint8_t dddat)
{
	uint32_t i=100;
	LL_USART_TransmitData8(USARTx, dddat);
	while(--i)
	{
		if(LL_USART_IsActiveFlag_TXE(USARTx)!=RESET){break;}
		delay_us(1);
	}
}
void send_data_all(USART_TypeDef *USARTx, uint8_t* dat,uint32_t len)
{
//	__disable_irq();
	uint32_t i=1000;
	for(uint32_t i=0;i<len;i++)	{	send_data_1(USARTx,dat[i]);	}

	while(--i)
	{
		if(LL_USART_IsActiveFlag_TC(USARTx)!=RESET){break;}
		delay_us(1);
	}
//	__enable_irq();
}
void pc_uart_send( uint8_t* dat,uint32_t len)
{
	send_data_all(UART4,dat,len);
}
void send_test(void)
{
	send_buf[0]=1;
	send_buf[1]=2;
	send_buf[2]=3;
	send_buf[3]=4;
	send_data_all(USART2, (uint8_t*)send_buf,4);
}



// 重定向printf到UART4
int __io_putchar(int ch)
{
//  HAL_UART_Transmit(&huart4, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
  send_data_1(UART4,(uint8_t)ch);
  return ch;
}

// 如果使用ARMCC编译器
int fputc(int ch, FILE *f)
{
  return __io_putchar(ch);
}
void usb_test(void)
{
	if (Appli_state == APPLICATION_START)
	{
		LL_GPIO_ResetOutputPin(GPIOC, LED_R_Pin);
		LL_GPIO_SetOutputPin(test_IO_GPIO_Port, test_IO_Pin);
	}
	else if (Appli_state == APPLICATION_READY)
	{
		LL_GPIO_ResetOutputPin(GPIOC, LED_B_Pin);
		CDC_TX_Buffer[0]=0x02;
		  CDC_TX_Buffer[1]=0x30;
		  CDC_TX_Buffer[2]=0x30;
		  CDC_TX_Buffer[3]=0x35;
		  CDC_TX_Buffer[4]=0x34;
		  CDC_TX_Buffer[5]=0x31;
		  CDC_TX_Buffer[6]=0x20;
		  CDC_TX_Buffer[7]=0x20;
		  CDC_TX_Buffer[8]=0x20;
		  CDC_TX_Buffer[9]=0x03;
		  CDC_TX_Buffer[10]=0x31;
		  CDC_TX_Buffer[11]=0x33;
		  CDC_TX_Buffer[12]=0x0D;
		  CDC_TX_Buffer[13]=0x0A;
		  USBH_FT232_Transmit (&hUsbHostFS, CDC_TX_Buffer, 14);
		  delay_ms(50);
		  USBH_FT232_Receive (&hUsbHostFS, CDC_RX_Buffer, RX_BUFF_SIZE);
		  delay_ms(50);
		  pc_uart_send(&CDC_RX_Buffer[2],RX_BUFF_SIZE);

	}
	else
	{
		if (Appli_state == APPLICATION_DISCONNECT){LL_GPIO_ResetOutputPin(test_IO_GPIO_Port, test_IO_Pin);}
		LL_GPIO_SetOutputPin(GPIOC, LED_B_Pin);
		LL_GPIO_SetOutputPin(GPIOC, LED_R_Pin);
	}

}

//	if (Appli_state == APPLICATION_START)
//	{
//		 if (Appli_state == APPLICATION_READY && line_coding_set == 0) {
//		            CDC_LineCodingTypeDef linecoding;
//		            linecoding.data_rate = 115200;    // 波特率115200
//		            linecoding.data_bits = 8;         // 8数据位
//		            linecoding.parity_type = 0;       // 无奇偶校验
//		            linecoding.char_format = 0;       // 1停止位
//
//		            if (USBH_CDC_SetLineCoding(&hUsbHostFS, &linecoding) == USBH_OK) {
//		                line_coding_set = 1;          // 设置成功，标记为已完成
//		            }
//		  USBH_CDC_Stop(&hUsbHostFS);
//		  CDC_TX_Buffer[0]=0x02;
//		  CDC_TX_Buffer[1]=0x30;
//		  CDC_TX_Buffer[2]=0x30;
//		  CDC_TX_Buffer[3]=0x35;
//		  CDC_TX_Buffer[4]=0x34;
//		  CDC_TX_Buffer[5]=0x31;
//		  CDC_TX_Buffer[6]=0x20;
//		  CDC_TX_Buffer[7]=0x20;
//		  CDC_TX_Buffer[8]=0x20;
//		  CDC_TX_Buffer[9]=0x03;
//		  CDC_TX_Buffer[10]=0x31;
//		  CDC_TX_Buffer[11]=0x33;
//		  CDC_TX_Buffer[12]=0x0D;
//		  CDC_TX_Buffer[13]=0x0A;
//		  USBH_CDC_Transmit (&hUsbHostFS, CDC_TX_Buffer, 14);
//		  LL_GPIO_ResetOutputPin(GPIOC, LED_B_Pin);
//	}
//	else
//	{
////	  USBH_CDC_Stop(&hUsbHostFS);
////	  usbresult = USBH_CDC_Receive(&hUsbHostFS, (uint8_t *) CDC_RX_Buffer, RX_BUFF_SIZE);
////	  HAL_Delay (200);
////	  pc_uart_send(CDC_RX_Buffer,RX_BUFF_SIZE);
////	  LL_GPIO_SetOutputPin(GPIOC, LED_B_Pin);
//	}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

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
  MX_UART4_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_USB_HOST_Init();
  /* USER CODE BEGIN 2 */
	LL_GPIO_SetOutputPin(GPIOC, LED_R_Pin|LED_B_Pin);
	LL_GPIO_ResetOutputPin(GPIOC, LED_R_Pin);
	set_voltage2(0);
	set_voltage1(0);
	USBH_UsrLog("power on\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)//18nS
  {
//		test();
//		if(KEY0_READ()==0){flag=flag?0:1;}
//		if(KEY1_READ()==0){flag=flag?0:1;}
//		if(KEY2_READ()==0){flag=flag?0:1;}
//		if(KEY3_READ()==0){flag=flag?0:1;}
//

//		HAL_Delay(500);
//		LL_GPIO_SetOutputPin(GPIOC, LED_R_Pin);
//		HAL_Delay(1);

    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

    /* USER CODE BEGIN 3 */


    if(time3_500ms_flag)
	{
		time3_500ms_flag=0;
		flag=flag?0:1;
//		if(flag)	{LL_GPIO_ResetOutputPin(GPIOC, LED_R_Pin);}
//		else		{LL_GPIO_SetOutputPin(GPIOC, LED_R_Pin);}
		usb_test();
	}
	if(and_uart_stop_cnt>5)
	{
		and_current_len_bk=and_current_len;
		my_copy((uint8_t*)and_rx_buffer_bk,(uint8_t*)and_rx_buffer,and_current_len_bk);
		and_uart_stop_cnt=0;
		and_current_len=0;
		//send_data_all(USART2,(uint8_t*)and_rx_buffer_bk,and_current_len_bk);
		crc_temp1=do_crc_table((uint8_t*)and_rx_buffer_bk,and_current_len_bk-2);
		temp2=crc_temp1;
		temp1=crc_temp1>>8;
		if((temp1==and_rx_buffer_bk[and_current_len_bk-2])&&(temp2==and_rx_buffer_bk[and_current_len_bk-1]))
		{
			send_data_all(USART2,(uint8_t*)and_rx_buffer_bk,and_current_len_bk-2);
			voltage1=((uint16_t)and_rx_buffer_bk[0])<<8;
			voltage1+=((uint16_t)and_rx_buffer_bk[1]);
			voltage2=((uint16_t)and_rx_buffer_bk[2])<<8;
			voltage2+=((uint16_t)and_rx_buffer_bk[3]);
			set_voltage2(voltage2);
			set_voltage1(voltage1);
		}
	}
	if(pc_uart_stop_cnt>5)
	{
		pc_current_len_bk=pc_current_len;
		my_copy((uint8_t*)pc_rx_buffer_bk,(uint8_t*)pc_rx_buffer,pc_current_len_bk);
		pc_uart_stop_cnt=0;
		pc_current_len=0;
		send_data_all(UART4,(uint8_t*)pc_rx_buffer_bk,pc_current_len_bk);
		crc_temp1=do_crc_table((uint8_t*)pc_rx_buffer_bk,pc_current_len_bk-2);
		temp2=crc_temp1;
		temp1=crc_temp1>>8;
		if((temp1==pc_rx_buffer_bk[pc_current_len_bk-2])&&(temp2==pc_rx_buffer_bk[pc_current_len_bk-1]))
		{
			send_data_all(USART2,(uint8_t*)pc_rx_buffer_bk,pc_current_len_bk-2);
		}
	}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
