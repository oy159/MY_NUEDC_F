/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include	"stdbool,h"
typedef enum {
	TSMS_SUCCESS = 0, // a successful operation
	TSMS_ERROR = 1,  // an error occurred ( one specific case of failure )
	TSMS_TIMEOUT = 2, // a timeout occurred ( one specific case of failure )
	TSMS_FAIL = 4 // a failed operation
} TSMS_RESULT;

#define	POSOFFSET	4*39
#define THREVOLT 0x200
#define XCORRSIZE 4 * 511




#define	bool	uint8_t
#define	true	1U
#define	false	0
#define	TSMS_malloc	malloc
#define	TSMS_NULL	NULL
#define	TSMS_delay	HAL_Delay
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define 	KEY1	(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9))
#define 	KEY2	(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5))
#define 	KEY3	(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12))
#define 	KEY4	(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11))
#define 	KEY5	(HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_11))


#define		LED1(n)	(n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_14,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_14,GPIO_PIN_RESET)) 
#define		LED2(n)	(n?HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET)) 
#define		LED3(n)	(n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_9,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_9,GPIO_PIN_RESET)) 
#define		LED4(n)	(n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_10,GPIO_PIN_RESET)) 


#define		CTRLCH4(n)	(n?HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET)) 

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern struct W25Q256_Handler *qspi_handler;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
