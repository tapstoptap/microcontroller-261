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
#define CLOCK_LED_MASK ((uint16_t)0xFFF0U)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void clearAllClock(void);
void setNumberOnClock(int num);
void clearNumberOnClock(int num);
static void displayClock(uint8_t hour, uint8_t minute, uint8_t second);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
 * LED order follows the clock positions.
 *
 * Position 0  = 12 o'clock = D2 = PA5
 * Position 1  = 1 o'clock  = D3 = PA6
 * Position 2  = 2 o'clock  = D4 = PA7
 * Position 3  = 3 o'clock  = D5 = PA8
 * Position 4  = 4 o'clock  = D6 = PA9
 * Position 5  = 5 o'clock  = D7 = PA10
 * Position 6  = 6 o'clock  = D8 = PA11
 * Position 7  = 7 o'clock  = D9 = PA12
 * Position 8  = 8 o'clock  = D10 = PA13
 * Position 9  = 9 o'clock  = D11 = PA14
 * Position 10 = 10 o'clock = D12 = PA15
 * Position 11 = 11 o'clock = D1 = PA4
 */
static const uint16_t clock_pins[12] =
{
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
    GPIO_PIN_4
};

/*
 * All LEDs are active-low:
 *
 * GPIO_PIN_RESET = LED ON
 * GPIO_PIN_SET   = LED OFF
 */
void clearAllClock(void)
{
    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_4  |
        GPIO_PIN_5  |
        GPIO_PIN_6  |
        GPIO_PIN_7  |
        GPIO_PIN_8  |
        GPIO_PIN_9  |
        GPIO_PIN_10 |
        GPIO_PIN_11 |
        GPIO_PIN_12 |
        GPIO_PIN_13 |
        GPIO_PIN_14 |
        GPIO_PIN_15,
        GPIO_PIN_SET
    );
}

void setNumberOnClock(int position)
{
    if (position >= 0 && position < 12)
    {
        HAL_GPIO_WritePin(
            GPIOA,
            clock_pins[position],
            GPIO_PIN_RESET
        );
    }
}

void clearNumberOnClock(int position)
{
    if (position >= 0 && position < 12)
    {
        HAL_GPIO_WritePin(GPIOA, clock_pins[position], GPIO_PIN_SET);
    }
}

static void displayClock(uint8_t hour, uint8_t minute, uint8_t second)
{
    const uint16_t on_pins = clock_pins[hour % 12U]
                           | clock_pins[minute / 5U]
                           | clock_pins[second / 5U];
    const uint16_t off_pins = CLOCK_LED_MASK & (uint16_t)~on_pins;

    /* Update all hands together; leave PA0-PA3 unchanged.
     * Low half sets OFF pins high; high half resets ON pins low.
     * Overlapping hands naturally share the same LED.
     */
    GPIOA->BSRR = (uint32_t)off_pins | ((uint32_t)on_pins << 16U);
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
  /* USER CODE BEGIN 2 */

    /* Start with every LED turned off */
    clearAllClock();

    /*
     * Start the simulated clock at 12:00:00.
     * At this time, D2 is the correct LED.
     */
    uint8_t hour = 12;
    uint8_t minute = 0;
    uint8_t second = 0;
    uint32_t previous_tick = HAL_GetTick();

    displayClock(hour, minute, second);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* Unsigned subtraction also handles the HAL tick wrapping.
         * Fixed deadlines avoid adding loop execution time each second.
         */
        while ((uint32_t)(HAL_GetTick() - previous_tick) >= 1000U)
        {
            previous_tick += 1000U;
            if (++second >= 60U)
            {
                second = 0;
                if (++minute >= 60U)
                {
                    minute = 0;
                    if (++hour > 12U)
                    {
                        hour = 1;
                    }
                }
            }
            displayClock(hour, minute, second);
        }
        HAL_Delay(1);
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);

  /*Configure GPIO pins : PA4 PA5 PA6 PA7
                           PA8 PA9 PA10 PA11
                           PA12 PA13 PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
