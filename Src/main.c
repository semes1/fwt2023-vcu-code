/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "cmsis_os.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "fatfs.h"
#include "iwdg.h"
#include "rtc.h"
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "wit_c_sdk.h"
#include "atk_m750.h"
#include "RingBuffer.h"
#include "string.h"
#include "gps.h"
#include "mtspeed.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/*******************************************
	*文件名 ：  U6_VCU
	*作   者：  罗成
	*修改时间： 2024.4.18
	*版   本：  v5.1.6
	*声   明：  本程序归飞翼车队所有，未经车队队长及指导老师允许，不能对外传播
	*提   示：  本程序采用Git进行版本管理，可通过git退回以前版本，但在选择时请选择
							最高版本（随版本递增，功能逐步递增）
							github仓库：https://github.com/TheorangeMr/U6xxxx.git
*******************************************/
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

extern uint8_t Oil_base_dat;
extern mt_rotate mtspeed1;
extern mt_rotate mtspeed2;
extern mt_rotate mtspeed3;
extern mt_rotate mtspeed4;
extern mt_rotate mtspeed5;
extern __IO uint16_t USART3_RX_STA;

uint8_t cantx_dat[8] = {0};
uint8_t time3_over1 = 0,time3_over2 = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
	函数名：Wit_Can_Send_Msg()
	功  能：CAN发送帧函数
  参  数：u8 ucStdId：扩展标识符, u8* msg：数据指针, u8 len：数据长度
  返回值：PASSED:传输成功，FAILED:失败
*/

void Wit_Can_Send_Msg(uint8_t ucStdId, uint8_t* msg, uint32_t len)
{
	uint16_t i=0;uint32_t can_tx_mailbox = 0;
	uint8_t canovertime = 0;
	CAN_TxHeaderTypeDef TxMessage;
	TxMessage.StdId=ucStdId;
	TxMessage.ExtId=0;
	TxMessage.IDE=CAN_ID_STD;
	TxMessage.RTR=CAN_RTR_DATA;
	TxMessage.DLC=len;
	TxMessage.TransmitGlobalTime=DISABLE;	
	for(i=0;i<len;i++)
	cantx_dat[i]=msg[i];
	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) < 1){
		if(canovertime++ >= 5){
			break;
		}else{
			osDelay(1);
		}
	}
	if(HAL_CAN_AddTxMessage(&hcan1,&TxMessage,cantx_dat,&can_tx_mailbox) != HAL_OK)
	{
		printf ("data sending failure！\r\n");
	}
}

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
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM14_Init();
  MX_IWDG_Init();
  MX_ADC2_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_TIM7_Init();
  MX_TIM13_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	
	//nine-axis dvice initialize
	WitInit(WIT_PROTOCOL_CAN, 0x50);
	WitRegisterCallBack(SensorDataUpdata);
	WitCanWriteRegister(Wit_Can_Send_Msg);
	WitDelayMsRegister(Wit_Delayms);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);
	
	BSP_SD_Init();
  printf("外设初始化\r\n");
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
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
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
	#define mt_cycle   300                     //mt算法采样周期
	#define reset_cycle   800                  //当不采样时的清零周期

	static uint8_t tim14_count = 0;
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  if (htim->Instance == TIM14) {
		if(mtspeed1.timecountflag){
			mtspeed1.timecount++;
			if(mtspeed1.timecount >= mt_cycle){
				mtspeed1.timecount = 0;
				mtspeed1.timecountflag = 0;
				mtspeed1.Endup_Flag = 1;                                           //规定周期采样结束标志			
			}
		}else{
			mtspeed1.speed_zero++;
			if(mtspeed1.speed_zero >= reset_cycle){
				mtspeed1.Rotate_Speed = 0;
			}
		}
		if(mtspeed2.timecountflag){
			mtspeed2.timecount++;
			if(mtspeed2.timecount >= mt_cycle){
				mtspeed2.timecountflag = 0;
				mtspeed2.timecount = 0;
				mtspeed2.Endup_Flag = 1;                                           //规定周期采样结束标志			
			}		
		}else{
			mtspeed2.speed_zero++;
			if(mtspeed2.speed_zero >= reset_cycle){
				mtspeed2.Rotate_Speed = 0;
			}
		}
		if(mtspeed3.timecountflag){
			mtspeed3.timecount++;
			if(mtspeed3.timecount >= mt_cycle){
				mtspeed3.timecountflag = 0;
				mtspeed3.timecount = 0;
				mtspeed3.Endup_Flag = 1;                                           //规定周期采样结束标志	
			}  
		}else{
			mtspeed3.speed_zero++;
			if(mtspeed3.speed_zero >= reset_cycle){
				mtspeed3.Rotate_Speed = 0;
			}
		}
		if(mtspeed4.timecountflag){
			mtspeed4.timecount++;
			if(mtspeed4.timecount >= mt_cycle){
				mtspeed4.timecountflag = 0;
				mtspeed4.timecount = 0;
				mtspeed4.Endup_Flag = 1;                                           //规定周期采样结束标志			
			}    
		}else{
			mtspeed4.speed_zero++;
			if(mtspeed4.speed_zero >= reset_cycle){
				mtspeed4.Rotate_Speed = 0;
			}
		}
		if(mtspeed5.timecountflag){
			mtspeed5.timecount++;
			if(mtspeed5.timecount >= mt_cycle){
				mtspeed5.timecountflag = 0;
				mtspeed5.timecount = 0;
				mtspeed5.Endup_Flag = 1;                                           //规定周期采样结束标志			
			}    
		}else{
			mtspeed5.speed_zero++;
			if(mtspeed5.speed_zero >= reset_cycle){
				mtspeed5.Rotate_Speed = 0;
			}
		}
	}
	if(htim->Instance == TIM3){
//		printf("timer3 timeout!!!\r\n");
		//time3计数器计数溢出，本次测速丢弃
		time3_over1 = 1;
		time3_over2 = 1;
	}
	if(htim->Instance == TIM7){
//		printf("timer7 timeout!!!\r\n");
		USART3_RX_STA|=1<<15;	//标记接收完成
		__HAL_TIM_DISABLE(&htim7);
	}
	if (htim->Instance == TIM13){
		tim14_count++;
		if(tim14_count >= 10)
		{
			if(Oil_base_dat > 0)
			{
				Oil_base_dat--;
			}
      tim14_count = 0;
		}
  }
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
	printf("Error_Handler\r\n");
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
