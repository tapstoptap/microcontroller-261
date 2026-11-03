/*
 * timer.c
 *
 * - timer_init()  : programs TIM2 for an interrupt every TIMER_CYCLE ms.
 * - setTimer() ...: software timers (non-blocking delays), durations in ms.
 * - HAL_TIM_PeriodElapsedCallback() : the ONLY thing the interrupt does is to
 *   advance the software timers and sample the buttons (debouncing).
 *   All the "real" work is done in the main loop (Exercise 8 style).
 */

#include "timer.h"
#include "input_reading.h"

extern TIM_HandleTypeDef htim2;     /* created by CubeMX in main.c */

#if (TIMER_CYCLE < 1) || (TIMER_CYCLE > 6553)
#error "TIMER_CYCLE must be between 1 and 6553 ms"
#endif

/* ---------------------------------------------------------------- */
/*  TIM2 configuration                                               */
/* ---------------------------------------------------------------- */
#define TIM_TICK_HZ     10000U      /* counter clock after the prescaler (0.1 ms) */

void timer_init(void)
{
    /* Timer input clock: APB1 timers run at 2 x PCLK1 when APB1 prescaler != 1 */
    uint32_t tim_clk = HAL_RCC_GetPCLK1Freq();
    if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1) {
        tim_clk *= 2U;
    }

    uint32_t psc = (tim_clk / TIM_TICK_HZ) - 1U;                 /* 8 MHz -> 799   */
    uint32_t arr = ((uint32_t)TIMER_CYCLE * (TIM_TICK_HZ / 1000U)) - 1U; /* 10 ms -> 99 */

    htim2.Init.Prescaler = psc;
    htim2.Init.Period    = arr;
    htim2.Instance->PSC  = psc;
    htim2.Instance->ARR  = arr;
    htim2.Instance->EGR  = TIM_EGR_UG;          /* load PSC/ARR immediately   */
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);

    HAL_TIM_Base_Start_IT(&htim2);
}

/* ---------------------------------------------------------------- */
/*  Software timers                                                  */
/* ---------------------------------------------------------------- */
static volatile int     timer_counter[NO_OF_TIMERS];
static volatile uint8_t timer_flag[NO_OF_TIMERS];

void setTimer(int index, int duration_ms)
{
    if (index < 0 || index >= NO_OF_TIMERS) return;

    /* number of interrupts to wait, rounded to the nearest tick, at least 1 */
    int ticks = (duration_ms + TIMER_CYCLE / 2) / TIMER_CYCLE;
    if (ticks < 1) ticks = 1;

    uint32_t primask = __get_PRIMASK();
    __disable_irq();                    /* the ISR also touches these      */
    timer_counter[index] = ticks;
    timer_flag[index]    = 0;
    __set_PRIMASK(primask);
}

int isTimerExpired(int index)
{
    if (index < 0 || index >= NO_OF_TIMERS) return 0;
    return timer_flag[index];
}

void timer_run(void)
{
    for (int i = 0; i < NO_OF_TIMERS; i++) {
        if (timer_counter[i] > 0) {
            timer_counter[i]--;
            if (timer_counter[i] == 0) timer_flag[i] = 1;
        }
    }
}

/* ---------------------------------------------------------------- */
/*  Interrupt callback (every TIMER_CYCLE ms)                        */
/* ---------------------------------------------------------------- */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) {
        timer_run();
        button_reading();
    }
}
