/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */


/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles CAN1 RX0 interrupt.
  */

void CAN1_RX0_IRQHandler(void)
{
  if (CAN1->RF0R & 0x3){
    uint32_t rxId = (CAN1->sFIFOMailBox[0].RIR >> 21) & 0x7FF;
    uint32_t data0 = CAN1->sFIFOMailBox[0].RDLR;
    uint32_t data1 = CAN1->sFIFOMailBox[0].RDHR;
    uint8_t dlc=CAN1->sFIFOMailBox[0].RDTR & 0xF;

    if (rxId==ID_MONITOR_HEARTBEAT){
      printf("MonitorHeartBeat: %d\r\n", data0 & 0xFF);
    }
    else if (rxId==ID_MOTOR_STATUS){
      uint8_t direction = data0 & 0xFF;
      uint16_t currentDuty = (data0>>8) & 0xFFFF;
      uint8_t errorFlag = (data0 >> 24) & 0xFF;
      printf("MotorStatus: dir=%d, duty=%d, err=%d\r\n",direction, currentDuty, errorFlag);
    }
    else if (rxId == ID_POT_VALUE){
      uint16_t rawAdc = data0 & 0xFFFF;
      printf("PotValue: %d\r\n", rawAdc);
    }
    else if (rxId == ID_DIAG_STATUS){
      uint8_t tec = data0 & 0xFF;
      uint8_t rec = (data0 >> 8) & 0xFF;
      uint8_t busState =  (data0 >> 16) & 0xFF;
      printf("DiagStatus: TEC=%d, REC=%d, state=%d\r\n", tec, rec, busState);
    }

    if (!IS_BUS_B_ID(rxId) && (CAN2->TSR & (1<<26))){ //버스 B 소속이 아닌 것만 CAN2로 전달 (루프 방지)
      CAN2->sTxMailBox[0].TIR  = (rxId<<21); // CAN1 소속 ID로 고정
      CAN2->sTxMailBox[0].TDTR = dlc;
      CAN2->sTxMailBox[0].TDLR = data0;
      CAN2->sTxMailBox[0].TDHR = data1;
      CAN2->sTxMailBox[0].TIR |= (1<<0);
    }
    CAN1->RF0R |= (1<<5);
  }
}

void CAN2_RX0_IRQHandler(void)
{
  if (CAN2->RF0R & 0x3){
    uint32_t rxId = (CAN2->sFIFOMailBox[0].RIR >> 21) & 0x7FF;
    uint32_t data0 = CAN2->sFIFOMailBox[0].RDLR;
    uint32_t data1 = CAN2->sFIFOMailBox[0].RDHR;
    
    //모터 조작 메시지 처리
    if (rxId==ID_MOTOR_COMMAND){
      uint8_t direction = data0 & 0xFF;
      uint16_t targetDuty = (data0 >> 8) & 0xFFFF;
      printf("MotorCommand: dir=%d duty=%d\r\n", direction, targetDuty);
      //phase 3.5에서 확장
    }
    //control heartbeat 이면 출력
    else if (rxId==ID_CONTROL_HEARTBEAT) printf("ControlHeartbeat: %d\r\n", data0 & 0xFF);

    if (!IS_BUS_A_ID(rxId) && (CAN1->TSR & (1<<26))){
      CAN1->sTxMailBox[0].TIR  = (rxId<<21); 
      CAN1->sTxMailBox[0].TDTR = CAN2->sFIFOMailBox[0].RDTR & 0xF;
      CAN1->sTxMailBox[0].TDLR = data0;
      CAN1->sTxMailBox[0].TDLR = data1;
      CAN1->sTxMailBox[0].TIR |= (1<<0);
    }
    CAN2->RF0R |= (1<<5);
  }

  /* USER CODE END CAN1_RX0_IRQn 0 */
  
  /* USER CODE BEGIN CAN1_RX0_IRQn 1 */

  /* USER CODE END CAN1_RX0_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
