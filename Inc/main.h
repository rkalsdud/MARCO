/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */


#define IS_BUS_A_ID(id) ((id)>=0x100 && (id)<=0x1FF) // 제어 버스
#define IS_BUS_B_ID(id) ((id)>=0x200 && (id)<=0x2FF) // 모니터링 버스

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

void Motor_Forward_Bare(void);
void Motor_Backward_Bare(void);
void Motor_Stop_Bare(void);
void Motor_SetSpeed_Bare(uint16_t duty);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define AIN0_Pin GPIO_PIN_0
#define AIN0_GPIO_Port GPIOC
#define AIN1_Pin GPIO_PIN_1
#define AIN1_GPIO_Port GPIOC
#define STBY_Pin GPIO_PIN_2
#define STBY_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define CAN_DATA_LEN 8
//제어 버스
#define ID_EMERGENCY_STOP 0x100
#define ID_MOTOR_COMMAND 0x101
#define ID_CONTROL_HEARTBEAT 0x1FF
//모니터링 버스
#define ID_MOTOR_STATUS 0x201
#define ID_POT_VALUE 0x202
#define ID_ENCODER_COUNT 0x203
#define ID_DIAG_STATUS 0x210
#define ID_MONITOR_HEARTBEAT 0x2FF
//방향
#define DIR_STOP     0
#define DIR_FORWARD  1
#define DIR_REVERSE  2
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
