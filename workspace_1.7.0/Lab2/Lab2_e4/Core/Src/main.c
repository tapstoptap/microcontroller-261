#include "main.h"

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

#define MAX_DIGITS                 4U
#define DIGIT_SLOT_TICKS           25U
#define DOT_TOGGLE_TICKS           100U

TIM_HandleTypeDef htim2;

static volatile uint16_t digit_tick_count = 0U;
static volatile uint16_t dot_tick_count = 0U;

const int MAX_LED = 4;
volatile int index_led = 0;
int led_buffer[4] = {6, 9, 7, 8};

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void disableAllDigits(void);
static void selectDigit(uint8_t index);
void display7SEG(int num);
void update7SEG(int index);

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

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_TIM2_Init();

  digit_tick_count = 0U;
  dot_tick_count = 0U;
  index_led = 0;

  HAL_GPIO_WritePin(AUX_LED_GPIO_PORT,
                    AUX_LED_PIN,
                    GPIO_PIN_SET);

  HAL_GPIO_WritePin(DOT_GPIO_PORT,
                    DOT_PIN,
                    GPIO_PIN_SET);

  update7SEG(index_led);

  index_led++;

  if (index_led >= MAX_LED)
  {
    index_led = 0;
  }

  if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  while (1)
  {
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK |
      RCC_CLOCKTYPE_SYSCLK |
      RCC_CLOCKTYPE_PCLK1 |
      RCC_CLOCKTYPE_PCLK2;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct,
                          FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

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

  if (HAL_TIM_ConfigClockSource(&htim2,
                                &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if (HAL_TIMEx_MasterConfigSynchronization(&htim2,
                                             &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA,
                    GPIO_PIN_4 |
                    GPIO_PIN_5 |
                    GPIO_PIN_6 |
                    GPIO_PIN_7 |
                    GPIO_PIN_8 |
                    GPIO_PIN_9,
                    GPIO_PIN_SET);

  HAL_GPIO_WritePin(GPIOB,
                    GPIO_PIN_0 |
                    GPIO_PIN_1 |
                    GPIO_PIN_2 |
                    GPIO_PIN_3 |
                    GPIO_PIN_4 |
                    GPIO_PIN_5 |
                    GPIO_PIN_6,
                    GPIO_PIN_SET);

  GPIO_InitStruct.Pin =
      GPIO_PIN_4 |
      GPIO_PIN_5 |
      GPIO_PIN_6 |
      GPIO_PIN_7 |
      GPIO_PIN_8 |
      GPIO_PIN_9;

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =
      GPIO_PIN_0 |
      GPIO_PIN_1 |
      GPIO_PIN_2 |
      GPIO_PIN_3 |
      GPIO_PIN_4 |
      GPIO_PIN_5 |
      GPIO_PIN_6;

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance != TIM2)
  {
    return;
  }

  digit_tick_count++;
  dot_tick_count++;

  if (digit_tick_count >= DIGIT_SLOT_TICKS)
  {
    digit_tick_count = 0U;

    update7SEG(index_led);

    index_led++;

    if (index_led >= MAX_LED)
    {
      index_led = 0;
    }
  }

  if (dot_tick_count >= DOT_TOGGLE_TICKS)
  {
    dot_tick_count = 0U;

    HAL_GPIO_TogglePin(DOT_GPIO_PORT,
                       DOT_PIN);
  }
}

void Error_Handler(void)
{
  __disable_irq();

  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file;
  (void)line;
}

#endif
