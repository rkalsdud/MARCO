/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//#define phase1
//#define phase2
#define phase3

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

TIM_HandleTypeDef htim3;
ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//  <------ phase 1------>

void Motor_Forward(void){
  HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(AIN0_GPIO_Port, AIN0_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_RESET);
}

void Motor_Backward(void){
  HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(AIN0_GPIO_Port, AIN0_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_SET);
}

void Motor_Stop(void){
  HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_RESET);
}

void Motor_Forward_Bare(void){
  GPIOC->BSRR=(1<<2); // STBY high
  GPIOC->BSRR=(1<<0); // AIN0 high
  GPIOC->BSRR=(1<<(1+16)); // AIN1 low
}

void Motor_Backward_Bare(void){
  GPIOC->BSRR=(1<<2); // STBY high
  GPIOC->BSRR=(1<<(0+16)); // AIN0 low
  GPIOC->BSRR=(1<<1); // AIN1 high
}

void Motor_Stop_Bare(void){
  GPIOC->BSRR=(1<<(2+16)); // STBY low
}

void Motor_SetSpeed(uint16_t duty){
  if (duty>999) duty=999;
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty);
}

void Motor_SetSpeed_Bare(uint16_t duty){
  if (duty>999) duty=999;
  TIM3->CCR1=duty;
}

uint16_t ADC_Read(void){
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
  uint16_t value = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  return value;
}

uint16_t ADC_Read_Bare(void){
  ADC1->CR2 |=(1<<30); // SWSTART=1: start conversion
  while (!(ADC1->SR & (1<<1))); // wait for EOC=1
  return (uint16_t)(ADC1->DR & 0xFFF); // read 12-bit value
}

uint16_t ADC_ToDuty(uint16_t adcValue){
  if (adcValue>4095) adcValue=4095;
  return (uint16_t)((uint32_t)adcValue*999/4095);
}

void TIM3_PWM_Init_Bare(void){
  GPIOA->MODER &= ~(0x3 << (6*2)); // clear [13:12]
  GPIOA->MODER |=(0x2<<(6*2)); // set [13:12] to 10: alternate function

  GPIOA->AFR[0]&=~(0xF<<(6*4)); // clear [27:24]
  GPIOA->AFR[0]|=(0x2<<(6*4)); // set [27:24] to 0010: AF2 (TIM3_CH1)

  RCC->APB1ENR|=(1<<1); // TIM3 clk enable

  TIM3->PSC=83; // prescaler
  TIM3->ARR=999; // auto reload

  TIM3->CCR1=500; // compare value

  TIM3->CCMR1 &= ~(0x7<<4); // clear OC1M[6:4]
  TIM3->CCMR1 |= (0x6 <<4); // OC1M=110: PWM mode 1
  TIM3->CCMR1 |= (1<<3); // OC1PE=1: preload enable

  TIM3->CCER |= (1<<0); // CC1E=1: output enable

  TIM3->CR1|=(1<<0); // CEN=1: counter enable
}

void ADC1_Init_Bare(void){
  GPIOC->MODER |=(0x3 << (3*2)); // set [7:6] to 11: analog mode for PC3 (ADC1_IN13)

  RCC->APB2ENR|=(1<<8); // ADC1 clk enable

  ADC1->SQR3=13; // 1st conversion in regular sequence is channel 13

  ADC1->SMPR1 &=~(0x7 << ((13-10)*3)); //clear [11:9] for channel 13

  ADC1->CR2 |=(1<<0); // ADON=1: enable ADC
}

void Encoder_TestPins_Init(void){
    // PA4 (하양): GPIOA 클럭은 MX_GPIO_Init()이 이미 켜둠
    GPIOA->MODER &= ~(0x3 << (4*2));  // Input 모드(00)
    GPIOA->PUPDR &= ~(0x3 << (4*2));
    GPIOA->PUPDR |=  (0x1 << (4*2)); // PULLUP

    // PC4 (초록): GPIOC 클럭도 이미 켜져있음
    GPIOC->MODER &= ~(0x3 << (4*2));
    GPIOC->PUPDR &= ~(0x3 << (4*2));
    GPIOC->PUPDR |=  (0x1 << (4*2)); // PULLUP
}

void Encoder_ExtClock_Init_Bare(void){
  GPIOA->MODER &= ~(0x3<<(0*2)); //PA0을 AF로 설정
  GPIOA->MODER |= (0x2<<(0*2));
  GPIOA->AFR[0] &= ~(0xF<<(0*4));
  GPIOA->AFR[0] |= (0x1<<(0*4)); //AF1 = TIM2

  GPIOA->PUPDR &= ~(0x3<<(0*2)); // PA0 풀업
  GPIOA->PUPDR|=(0x1<<(0*2));

  RCC->APB1ENR |= (1<<0); //TIM2 클럭 활성화

  //TIM2 를 External Clock Mode 1 로 설정: CH1 엣지마다 CNT 증가
  TIM2->SMCR &=~(0x7<<4); 
  TIM2->SMCR |= (0x5<<4);
  TIM2->SMCR &= ~(0x7<<0); 
  TIM2->SMCR |= (0x7<<0);

  TIM2->CCMR1 &=~(0x3<<0); 
  TIM2->CCMR1 |= (0x1<<0);

  // TIM2->CCMR1 &= ~(0xF<<4); 
  // TIM2->CCMR1 |= (0x3<<4);

  TIM2->ARR=0xFFFF; //최대 카운트
  TIM2->CR1 |= (1<<0); //CEN=1; 카운터 시작
}

uint16_t Encoder_GetPulseCount(void){
  return (uint16_t)TIM2->CNT;
}

float kp = 1.0f;
float ki=0.1f;
float kd= 0.0f;

float pid_integral= 0.0f;
float pid_prevError = 0.0f;

uint16_t PID_Compute(int32_t targetPulses, int32_t measuredPulses, float dt){
  float error = (float)(targetPulses - measuredPulses);

  pid_integral+=error*dt;
  if (pid_integral>2000.0f) pid_integral=2000.0f;
  if (pid_integral<-2000.0f) pid_integral=-2000.0f;

  float derivative = (error - pid_prevError) / dt;
  pid_prevError = error;
  
  float output=kp*error + ki*pid_integral + kd*derivative;

  static float currentDuty=0;
  currentDuty+=output*dt;

  if (currentDuty>999) currentDuty=999;
  if (currentDuty<0) currentDuty=0;

  return (uint16_t)currentDuty;
}

//  <------ phase 1------>

//  <------- phase2 ------>

void DWT_Init(void) { //DWT 사이클 카운터 초기화
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 트레이스 활성화
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            // 카운터 시작
}

uint32_t DWT_GetCycles(void) {//DWT 사이클 카운터 측정
    return DWT->CYCCNT;
}

uint16_t ADC_ToDuty_Quantized(uint16_t adcValue) {// 양자화 기법으로 ADC_ToDuty 함수 최적화
    if (adcValue > 4095) adcValue = 4095;
    const uint32_t SCALE_Q16 = 15984;
    uint16_t duty = (uint16_t)(((uint32_t)adcValue * SCALE_Q16) >> 16);
    if (duty > 999) duty = 999;  // ★ 양자화 오차로 인한 상한 초과 방어
    return duty;
}

#define FILTER_SIZE 8
uint16_t filterBuf[FILTER_SIZE] = {0};
uint8_t filterIdx = 0;

uint16_t ADC_MovingAverage(uint16_t newValue){//이동 평균 필터
    filterBuf[filterIdx] = newValue;
    filterIdx = (filterIdx + 1) % FILTER_SIZE;

    uint32_t sum = 0;
    for (int i = 0; i < FILTER_SIZE; i++) sum += filterBuf[i];
    return (uint16_t)(sum / FILTER_SIZE);
}

//  <-------phase 2------>

//  <-------phase 3------>

void CAN1_Init_Bare(void) {
    GPIOB->MODER &= ~((0x3 << (8 * 2)) | (0x3 << (9 * 2))); // clear [17:16] and [19:18]
    GPIOB->MODER |= ((0x2 << (8 * 2)) | (0x2 << (9 * 2))); // set [17:16] and [19:18] to 10: alternate function

    GPIOB->AFR[1] &= ~((0xF << (0 * 4)) | (0xF << (1 * 4))); // clear [3:0] and [7:4]
    GPIOB->AFR[1] |= ((0x9 << (0 * 4)) | (0x9 << (1 * 4))); // set [3:0] and [7:4] to 1001: AF9 (CAN1)

    GPIOB->PUPDR &= ~(0x3 << (8 * 2)); // clear [17:16] (no pull-up/pull-down)
    GPIOB->PUPDR |= (0x1 << (8 * 2)); // set [19:18] to 01: pull-up for CAN1_RX

    RCC->APB1ENR |= (1 << 25); // CAN1 clk enable

    CAN1->MCR |= (1 << 0); // INRQ=1: init mode
    CAN1->MCR &= ~(1 << 1); // sleep mode disable
    while (!(CAN1->MSR & (1 << 0))); // wait for INAK=1

    CAN1->BTR = (0 << 30)   // LBKM=Normal mode
        | (0 << 24)   // SJW=1TQ
        | (1 << 20)   // TS2=2TQ
        | (10 << 16)  // TS1=11TQ
        | (5 << 0);   // BRP=6

    CAN1->MCR &= ~(1 << 4); //NART=0: AutoRetransmission enable

    CAN1->FMR |= (1 << 0); //FINIT=1: filter init mode
    CAN1->FA1R &= ~(1 << 0); // deactivate filter 0

    CAN1->sFilterRegister[0].FR1 = 0x00000000; // filter id
    CAN1->sFilterRegister[0].FR2 = 0x00000000; // filter mask
    CAN1->FA1R |= (1 << 0); // activate filter 0
    CAN1->FMR &= ~(1 << 0); //FINIT=0: filter active

    CAN1->MCR &= ~(1 << 0); // INRQ=0: normal mode
    while (CAN1->MSR & (1 << 0)); // wait for INAK=0

    CAN1->IER |= (1 << 1); // enable FIFO0 message pending interrupt
    NVIC_EnableIRQ(CAN1_RX0_IRQn); // enable CAN1 RX0 interrupt in NVIC
    NVIC_SetPriority(CAN1_RX0_IRQn, 0); // set priority to 0
}

void CAN2_Init_Bare(void) {
    GPIOB->MODER &= ~((0x3 << (5 * 2) | (0x3 << (6 * 2)))); // clear [11:10] and [13:12]
    GPIOB->MODER |= ((0x2 << 5 * 2) | (0x2 << (6 * 2))); // set [11:10] and [13:12] to 10: alternate function

    GPIOB->AFR[0] &= ~((0xF << (5 * 4)) | (0xF << (6 * 4))); // clear [23:20] and [27:24]
    GPIOB->AFR[0] |= ((0x9 << (5 * 4)) | (0x9 << (6 * 4))); // set [23:20] and [27:24] to 1001: AF9 (CAN2)

    GPIOB->PUPDR &= ~(0x3 << (5 * 2)); // clear [11:10] (no pull-up/pull-down)
    GPIOB->PUPDR |= (0x1 << (5 * 2)); // set [11:10] to 01: pull-up for CAN2_RX

    RCC->APB1ENR |= (1 << 25); // CAN1 clk enable
    RCC->APB1ENR |= (1 << 26); // CAN2 clk enable

    CAN2->MCR |= (1 << 0); // INRQ=1: init mode
    CAN2->MCR &= ~(1 << 1); // sleep mode disable
    while (!(CAN2->MSR & (1 << 0))); // wait for INAK=1

    CAN2->BTR = (0 << 30)   // LBKM: normal mode
        | (0 << 24)   // SJW=1TQ
        | (1 << 20)   // TS2=2TQ
        | (10 << 16)  // TS1=11TQ
        | (5 << 0);   // BRP=6

    CAN2->MCR &= ~(1 << 4); //NART=0: AutoRetransmission enable

    CAN1->FMR |= (1 << 0); //FINIT=1: filter init mode
    CAN1->FMR &= ~(0x3F << 8); // clear CANSB (CAN start bank)
    CAN1->FMR |= (1 << 8); // set CANSB=1: CAN2 filters start from bank 1

    CAN1->FA1R &= ~(1 << 1); // deactivate filter 1 (CAN2 filter)
    CAN1->sFilterRegister[1].FR1 = 0x00000000; // filter id
    CAN1->sFilterRegister[1].FR2 = 0x00000000; // filter mask
    CAN1->FA1R |= (1 << 1); // activate filter 1 (CAN2 filter)
    CAN1->FMR &= ~(1 << 0); //FINIT=0: filter active

    CAN2->MCR &= ~(1 << 0); // INRQ=0: normal mode
    while (CAN2->MSR & (1 << 0)); // wait for INAK=0

    CAN2->IER |= (1 << 1); // enable FIFO0 message pending interrupt
    NVIC_EnableIRQ(CAN2_RX0_IRQn); // enable CAN2 RX0 interrupt in NVIC
    NVIC_SetPriority(CAN2_RX0_IRQn, 0); // set priority to 0
}

//  <-------phase 3------>

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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  TIM3_PWM_Init_Bare();
  CAN1_Init_Bare();
  CAN2_Init_Bare();
  ADC1_Init_Bare();
  Encoder_TestPins_Init();
  DWT_Init();
  Encoder_ExtClock_Init_Bare();

#ifdef phase2 
  ////    <------ HAL vs BareMetal ------>
  // GPIO
  uint32_t cycles_hal = 0, cycles_bare = 0;

  for (int i = 0; i < 100; i++) {
      uint32_t s = DWT_GetCycles();
      Motor_Forward();
      uint32_t e = DWT_GetCycles();
      cycles_hal += (e - s);
  }
  for (int i = 0; i < 100; i++) {
      uint32_t s = DWT_GetCycles();
      Motor_Forward_Bare();
      uint32_t e = DWT_GetCycles();
      cycles_bare += (e - s);
  }
  printf("HAL avg: %lu cycles\r\n", cycles_hal / 100);
  printf("Bare avg: %lu cycles\r\n", cycles_bare / 100);

  // PWM
  uint32_t cycles_pwm_hal = 0, cycles_pwm_bare = 0;

  for (int i = 0; i < 100; i++) {
      uint32_t s = DWT_GetCycles();
      Motor_SetSpeed(500);
      uint32_t e = DWT_GetCycles();
      cycles_pwm_hal += (e - s);
  }
  for (int i = 0; i < 100; i++) {
      uint32_t s = DWT_GetCycles();
      Motor_SetSpeed_Bare(500);
      uint32_t e = DWT_GetCycles();
      cycles_pwm_bare += (e - s);
  }
  printf("PWM HAL avg: %lu cycles\r\n", cycles_pwm_hal / 100);
  printf("PWM Bare avg: %lu cycles\r\n", cycles_pwm_bare / 100);
  //    <------ HAL vs BareMetal ------>

  //    <------ 양자화 적용 ------>
  uint32_t cycles_div = 0, cycles_quant = 0;
  uint16_t dummy_inputs[5] = { 0, 1000, 2048, 3000, 4095 };

  for (int i = 0; i < 100; i++) {
      for (int j = 0; j < 5; j++) {
          uint32_t s = DWT_GetCycles();
          volatile uint16_t r = ADC_ToDuty(dummy_inputs[j]);
          uint32_t e = DWT_GetCycles();
          cycles_div += (e - s);
      }
  }
  for (int i = 0; i < 100; i++) {
      for (int j = 0; j < 5; j++) {
          uint32_t s = DWT_GetCycles();
          volatile uint16_t r = ADC_ToDuty_Quantized(dummy_inputs[j]);
          uint32_t e = DWT_GetCycles();
          cycles_quant += (e - s);
      }
  }
  printf("Div avg: %lu cycles\r\n", cycles_div / 500);
  printf("Quant avg: %lu cycles\r\n", cycles_quant / 500);

  // 오차 검증 (0~4095 전수 비교)
  int16_t max_error = 0;
  for (uint32_t v = 0; v <= 4095; v++) {
      int16_t diff = (int16_t)ADC_ToDuty((uint16_t)v)
          - (int16_t)ADC_ToDuty_Quantized((uint16_t)v);
      if (diff < 0) diff = -diff;
      if (diff > max_error) max_error = diff;
  }
  printf("Max error: %d\r\n", max_error);
  //    <------ 양자화 적용 ------>

#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
#ifdef phase1
     uint16_t adcValue=ADC_Read_Bare();
     int32_t targetPulses=((uint32_t)adcValue*2500)/4095; //0~2500 범위로 만듬

     Motor_Forward_Bare();

     uint16_t before=Encoder_GetPulseCount();
     HAL_Delay(200); 
     uint16_t after=Encoder_GetPulseCount();
     int32_t measuredPulses = (uint16_t)(after-before);
     measuredPulses=measuredPulses*5; //200ms -> 1s 

     uint16_t duty=PID_Compute(targetPulses, measuredPulses, 0.2f);
     Motor_SetSpeed_Bare(duty);

      printf("target=%ld measured=%ld duty=%d\r\n", targetPulses, measuredPulses, duty);
#endif

#ifdef phase3
    static int counter=0;
    counter++;
    //제어 버스 송신
    if (CAN1->TSR & (1<<26)){//tx mailbox 0 empty
      if (counter%5 !=0){
        uint8_t direction=DIR_FORWARD; // 임시적으로 정방향 고정
        uint16_t adcValue=ADC_Read_Bare();
        uint16_t targetDuty=ADC_ToDuty(adcValue);

        CAN1->sTxMailBox[0].TIR=(ID_MOTOR_COMMAND<<21);
        CAN1->sTxMailBox[0].TDTR=3; //DLC=3
        CAN1->sTxMailBox[0].TDLR=direction|((uint32_t)targetDuty<<8);
        CAN1->sTxMailBox[0].TIR|=(1<<0); //transmission
      }
      else{
        CAN1->sTxMailBox[0].TIR=(ID_CONTROL_HEARTBEAT<<21);
        CAN1->sTxMailBox[0].TDTR=1; //DLC=1
        CAN1->sTxMailBox[0].TDLR=(uint8_t)counter;
        CAN1->sTxMailBox[0].TIR|=(1<<0);
      }
    }

    //모니터링 버스 송신
    if (CAN2->TSR & (1<<26)){ // CAN2 메일박스0 비어있는지 확인
      CAN2->sTxMailBox[0].TIR  = (ID_MONITOR_HEARTBEAT<<21); // CAN1의 0x123과 다른 ID로 구분
      CAN2->sTxMailBox[0].TDTR = 1;
      CAN2->sTxMailBox[0].TDLR = (uint8_t)counter;
      CAN2->sTxMailBox[0].TIR |= (1<<0);
    }
#endif

    HAL_Delay(1000);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */

static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, AIN0_Pin|AIN1_Pin|STBY_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : AIN0_Pin AIN1_Pin STBY_Pin */
  GPIO_InitStruct.Pin = AIN0_Pin|AIN1_Pin|STBY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */



int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

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
#ifdef USE_FULL_ASSERT
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
