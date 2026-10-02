/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
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

#define SEG_A_PIN                  GPIO_PIN_0
#define SEG_B_PIN                  GPIO_PIN_1
#define SEG_C_PIN                  GPIO_PIN_2
#define SEG_D_PIN                  GPIO_PIN_3
#define SEG_E_PIN                  GPIO_PIN_4
#define SEG_F_PIN                  GPIO_PIN_5
#define SEG_G_PIN                  GPIO_PIN_6
#define SEGMENT_GPIO_PORT          GPIOB

#define SEGMENT_PINS              (SEG_A_PIN | SEG_B_PIN | SEG_C_PIN | \
                                   SEG_D_PIN | SEG_E_PIN | SEG_F_PIN | \
                                   SEG_G_PIN)

#define EN0_PIN                    GPIO_PIN_6
#define EN1_PIN                    GPIO_PIN_7
#define EN2_PIN                    GPIO_PIN_8
#define EN3_PIN                    GPIO_PIN_9
#define DIGIT_ENABLE_GPIO_PORT     GPIOA

#define DIGIT_ENABLE_PINS         (EN0_PIN | EN1_PIN | EN2_PIN | EN3_PIN)

#define DOT_PIN                    GPIO_PIN_4
#define DOT_GPIO_PORT              GPIOA

#define AUX_LED_PIN                GPIO_PIN_5
#define AUX_LED_GPIO_PORT          GPIOA

#define ENM0_PIN                   GPIO_PIN_2
#define ENM1_PIN                   GPIO_PIN_3
#define ENM2_PIN                   GPIO_PIN_10
#define ENM3_PIN                   GPIO_PIN_11
#define ENM4_PIN                   GPIO_PIN_12
#define ENM5_PIN                   GPIO_PIN_13
#define ENM6_PIN                   GPIO_PIN_14
#define ENM7_PIN                   GPIO_PIN_15
#define MATRIX_ENABLE_GPIO_PORT    GPIOA

#define MATRIX_ENABLE_PINS        (ENM0_PIN | ENM1_PIN | ENM2_PIN | \
                                   ENM3_PIN | ENM4_PIN | ENM5_PIN | \
                                   ENM6_PIN | ENM7_PIN)

#define MATRIX_ROW_GPIO_PORT       GPIOB
#define MATRIX_ROW_PINS           (GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
                                   GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | \
                                   GPIO_PIN_14 | GPIO_PIN_15)

#define MAX_DIGITS                 4U
#define MAX_MATRIX_COLUMNS         8U
#define MATRIX_PATTERN_COLUMNS     16U
#define DIGIT_SLOT_MS              250
#define MATRIX_COLUMN_PERIOD_MS    10
#define ANIMATION_PERIOD_MS        200
#define TIMER_CYCLE                10

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

const int MAX_LED = 4;
volatile int index_led = 0;
volatile int led_buffer[4] = {0, 0, 0, 0};

const int MAX_LED_MATRIX = 8;
volatile int index_led_matrix = 0;
uint8_t matrix_buffer[8] =
{
  0x00U,
  0x7EU,
  0x09U,
  0x09U,
  0x09U,
  0x7EU,
  0x00U,
  0x00U
};

static const uint8_t animation_pattern[MATRIX_PATTERN_COLUMNS] =
{
  0x00U,
  0x7EU,
  0x09U,
  0x09U,
  0x09U,
  0x7EU,
  0x00U,
  0x00U,
  0x00U,
  0x00U,
  0x00U,
  0x00U,
  0x00U,
  0x00U,
  0x00U,
  0x00U
};

uint8_t animation_offset = 0U;

int hour = 15;
int minute = 8;
int second = 50;

volatile int timer0_counter = 0;
volatile int timer0_flag = 0;
volatile int timer1_counter = 0;
volatile int timer1_flag = 0;
volatile int timer2_counter = 0;
volatile int timer2_flag = 0;
volatile int timer3_counter = 0;
volatile int timer3_flag = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

static void disableAllDigits(void);
static void selectDigit(uint8_t index);
static void disableAllMatrixColumns(void);
static void selectMatrixColumn(uint8_t index);
static void writeMatrixRows(uint8_t pattern);
void display7SEG(int num);
void update7SEG(int index);
void updateClockBuffer(void);
void updateLEDMatrix(int index);
void updateAnimationBuffer(void);
void setTimer0(int duration);
void setTimer1(int duration);
void setTimer2(int duration);
void setTimer3(int duration);
void timer_run(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void disableAllDigits(void)
{
  HAL_GPIO_WritePin(DIGIT_ENABLE_GPIO_PORT,
                    DIGIT_ENABLE_PINS,
                    GPIO_PIN_SET);
}

static void selectDigit(uint8_t index)
{
  static const uint16_t enable_pins[MAX_DIGITS] =
  {
    EN0_PIN,
    EN1_PIN,
    EN2_PIN,
    EN3_PIN
  };

  if (index < MAX_DIGITS)
  {
    HAL_GPIO_WritePin(DIGIT_ENABLE_GPIO_PORT,
                      enable_pins[index],
                      GPIO_PIN_RESET);
  }
}

static void disableAllMatrixColumns(void)
{
  HAL_GPIO_WritePin(MATRIX_ENABLE_GPIO_PORT,
                    MATRIX_ENABLE_PINS,
                    GPIO_PIN_SET);
}

static void selectMatrixColumn(uint8_t index)
{
  static const uint16_t enable_pins[MAX_MATRIX_COLUMNS] =
  {
    ENM0_PIN,
    ENM1_PIN,
    ENM2_PIN,
    ENM3_PIN,
    ENM4_PIN,
    ENM5_PIN,
    ENM6_PIN,
    ENM7_PIN
  };

  if (index < MAX_MATRIX_COLUMNS)
  {
    HAL_GPIO_WritePin(MATRIX_ENABLE_GPIO_PORT,
                      enable_pins[index],
                      GPIO_PIN_RESET);
  }
}

static void writeMatrixRows(uint8_t pattern)
{
  uint16_t on_pins;
  uint16_t off_pins;

  on_pins = ((uint16_t)pattern << 8U) & MATRIX_ROW_PINS;
  off_pins = MATRIX_ROW_PINS & (uint16_t)(~on_pins);

  MATRIX_ROW_GPIO_PORT->BSRR =
      (uint32_t)off_pins |
      ((uint32_t)on_pins << 16U);
}

void display7SEG(int num)
{
  static const uint16_t digit_pattern[10] =
  {
    0x3FU,
    0x06U,
    0x5BU,
    0x4FU,
    0x66U,
    0x6DU,
    0x7DU,
    0x07U,
    0x7FU,
    0x6FU
  };

  uint16_t on_pins;
  uint16_t off_pins;

  if ((num < 0) || (num > 9))
  {
    HAL_GPIO_WritePin(SEGMENT_GPIO_PORT,
                      SEGMENT_PINS,
                      GPIO_PIN_SET);
    return;
  }

  on_pins = digit_pattern[num] & SEGMENT_PINS;
  off_pins = SEGMENT_PINS & (uint16_t)(~on_pins);

  SEGMENT_GPIO_PORT->BSRR =
      (uint32_t)off_pins |
      ((uint32_t)on_pins << 16U);
}

void update7SEG(int index)
{
  disableAllDigits();

  switch (index)
  {
    case 0:
      display7SEG(led_buffer[0]);
      selectDigit(0U);
      break;

    case 1:
      display7SEG(led_buffer[1]);
      selectDigit(1U);
      break;

    case 2:
      display7SEG(led_buffer[2]);
      selectDigit(2U);
      break;

    case 3:
      display7SEG(led_buffer[3]);
      selectDigit(3U);
      break;

    default:
      break;
  }
}

void updateClockBuffer(void)
{
  led_buffer[0] = hour / 10;
  led_buffer[1] = hour % 10;
  led_buffer[2] = minute / 10;
  led_buffer[3] = minute % 10;
}

void updateLEDMatrix(int index)
{
  disableAllMatrixColumns();
  writeMatrixRows(0x00U);

  switch (index)
  {
    case 0:
      writeMatrixRows(matrix_buffer[0]);
      selectMatrixColumn(0U);
      break;

    case 1:
      writeMatrixRows(matrix_buffer[1]);
      selectMatrixColumn(1U);
      break;

    case 2:
      writeMatrixRows(matrix_buffer[2]);
      selectMatrixColumn(2U);
      break;

    case 3:
      writeMatrixRows(matrix_buffer[3]);
      selectMatrixColumn(3U);
      break;

    case 4:
      writeMatrixRows(matrix_buffer[4]);
      selectMatrixColumn(4U);
      break;

    case 5:
      writeMatrixRows(matrix_buffer[5]);
      selectMatrixColumn(5U);
      break;

    case 6:
      writeMatrixRows(matrix_buffer[6]);
      selectMatrixColumn(6U);
      break;

    case 7:
      writeMatrixRows(matrix_buffer[7]);
      selectMatrixColumn(7U);
      break;

    default:
      break;
  }
}

void updateAnimationBuffer(void)
{
  uint8_t column;

  for (column = 0U; column < MAX_MATRIX_COLUMNS; column++)
  {
    matrix_buffer[column] =
        animation_pattern[(animation_offset + column) %
                          MATRIX_PATTERN_COLUMNS];
  }
}

void setTimer0(int duration)
{
  timer0_counter = duration / TIMER_CYCLE;
  timer0_flag = 0;
}

void setTimer1(int duration)
{
  timer1_counter = duration / TIMER_CYCLE;
  timer1_flag = 0;
}

void setTimer2(int duration)
{
  timer2_counter = duration / TIMER_CYCLE;
  timer2_flag = 0;
}

void setTimer3(int duration)
{
  timer3_counter = duration / TIMER_CYCLE;
  timer3_flag = 0;
}

void timer_run(void)
{
  if (timer0_counter > 0)
  {
    timer0_counter--;
  }

  if (timer0_counter == 0)
  {
    timer0_flag = 1;
  }

  if (timer1_counter > 0)
  {
    timer1_counter--;
  }

  if (timer1_counter == 0)
  {
    timer1_flag = 1;
  }

  if (timer2_counter > 0)
  {
    timer2_counter--;
  }

  if (timer2_counter == 0)
  {
    timer2_flag = 1;
  }

  if (timer3_counter > 0)
  {
    timer3_counter--;
  }

  if (timer3_counter == 0)
  {
    timer3_flag = 1;
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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  index_led = 0;
  index_led_matrix = 0;

  HAL_GPIO_WritePin(AUX_LED_GPIO_PORT,
                    AUX_LED_PIN,
                    GPIO_PIN_SET);

  HAL_GPIO_WritePin(DOT_GPIO_PORT,
                    DOT_PIN,
                    GPIO_PIN_SET);

  disableAllMatrixColumns();
  writeMatrixRows(0x00U);

  animation_offset = 0U;
  updateAnimationBuffer();

  updateClockBuffer();
  update7SEG(index_led);

  index_led++;

  if (index_led >= MAX_LED)
  {
    index_led = 0;
  }

  updateLEDMatrix(index_led_matrix);

  index_led_matrix++;

  if (index_led_matrix >= MAX_LED_MATRIX)
  {
    index_led_matrix = 0;
  }

  setTimer0(1000);
  setTimer1(DIGIT_SLOT_MS);
  setTimer2(MATRIX_COLUMN_PERIOD_MS);
  setTimer3(ANIMATION_PERIOD_MS);

  if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    if (timer0_flag == 1)
    {
      second++;

      if (second >= 60)
      {
        second = 0;
        minute++;
      }

      if (minute >= 60)
      {
        minute = 0;
        hour++;
      }

      if (hour >= 24)
      {
        hour = 0;
      }

      updateClockBuffer();
      HAL_GPIO_TogglePin(DOT_GPIO_PORT, DOT_PIN);
      setTimer0(1000);
    }

    if (timer1_flag == 1)
    {
      update7SEG(index_led);

      index_led++;

      if (index_led >= MAX_LED)
      {
        index_led = 0;
      }

      setTimer1(DIGIT_SLOT_MS);
    }

    if (timer3_flag == 1)
    {
      animation_offset++;

      if (animation_offset >= MATRIX_PATTERN_COLUMNS)
      {
        animation_offset = 0U;
      }

      updateAnimationBuffer();
      setTimer3(ANIMATION_PERIOD_MS);
    }

    if (timer2_flag == 1)
    {
      updateLEDMatrix(index_led_matrix);

      index_led_matrix++;

      if (index_led_matrix >= MAX_LED_MATRIX)
      {
        index_led_matrix = 0;
      }

      setTimer2(MATRIX_COLUMN_PERIOD_MS);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_SET);

  /*Configure GPIO pins : PA2 PA3 PA4 PA5
                           PA6 PA7 PA8 PA9
                           PA10 PA11 PA12 PA13
                           PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB12 PB13 PB14
                           PB15 PB3 PB4 PB5
                           PB6 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance != TIM2)
  {
    return;
  }

  timer_run();
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
