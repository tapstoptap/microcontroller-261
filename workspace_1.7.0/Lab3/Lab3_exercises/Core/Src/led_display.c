/*
 * led_display.c  -  output driver of the "input / output processing" pattern
 *
 * FSM (main loop) --> led_buffer[] / setTrafficLeds() --> this module --> pins
 */

#include "main.h"
#include "global.h"
#include "timer.h"
#include "led_display.h"

/* ------------------------------------------------------------------ */
/*  7-segment display                                                  */
/* ------------------------------------------------------------------ */
int led_buffer[MAX_LED] = { 0, 0, 0, 0 };
static int index_led = 0;

static const uint16_t SEG_PIN[7] = {
    SEG_A_PIN, SEG_B_PIN, SEG_C_PIN, SEG_D_PIN, SEG_E_PIN, SEG_F_PIN, SEG_G_PIN
};
static const uint16_t DIGIT_PIN[MAX_LED] = {
    DIGIT1_PIN, DIGIT2_PIN, DIGIT3_PIN, DIGIT4_PIN
};

/* bit0 = a ... bit6 = g, 1 = segment lit */
static const uint8_t SEG_CODE[10] = {
    0x3F,   /* 0 : a b c d e f   */
    0x06,   /* 1 :   b c         */
    0x5B,   /* 2 : a b   d e   g */
    0x4F,   /* 3 : a b c d     g */
    0x66,   /* 4 :   b c     f g */
    0x6D,   /* 5 : a   c d   f g */
    0x7D,   /* 6 : a   c d e f g */
    0x07,   /* 7 : a b c         */
    0x7F,   /* 8 : all           */
    0x6F    /* 9 : a b c d   f g */
};

/* Put one decimal digit (0..9) on the segment lines. Anything else = blank. */
void display7SEG(int num)
{
    uint8_t code = (num >= 0 && num <= 9) ? SEG_CODE[num] : 0x00;
    for (int i = 0; i < 7; i++) {
        HAL_GPIO_WritePin(SEG_PORT, SEG_PIN[i],
                          ((code >> i) & 1U) ? SEG_ON_LEVEL : SEG_OFF_LEVEL);
    }
}

static void enableDigit(int index)           /* exactly one digit on, others off */
{
    for (int i = 0; i < MAX_LED; i++) {
        HAL_GPIO_WritePin(DIGIT_PORT, DIGIT_PIN[i],
                          (i == index) ? DIGIT_ON_LEVEL : DIGIT_OFF_LEVEL);
    }
}

/* Show led_buffer[index] on digit "index" (0..3). */
void update7SEG(int index)
{
    if (index < 0 || index >= MAX_LED) return;

    for (int i = 0; i < MAX_LED; i++) {                    /* 1) all digits off   */
        HAL_GPIO_WritePin(DIGIT_PORT, DIGIT_PIN[i], DIGIT_OFF_LEVEL);
    }
    display7SEG(led_buffer[index]);                        /* 2) new segments     */
    enableDigit(index);                                    /* 3) enable this digit */
}

/* left (0..99) -> digits 1-2, right (0..99) -> digits 3-4, leading zeros kept */
static int clamp99(int v)
{
    if (v < 0)  return 0;
    if (v > 99) return 99;
    return v;
}

void updateLedBuffer(int left, int right)
{
    left  = clamp99(left);
    right = clamp99(right);
    led_buffer[0] = left  / 10;
    led_buffer[1] = left  % 10;
    led_buffer[2] = right / 10;
    led_buffer[3] = right % 10;
}

/* ------------------------------------------------------------------ */
/*  Traffic lights                                                     */
/* ------------------------------------------------------------------ */
static const uint16_t ROAD_PIN[2][4] = {
    /* OFF  RED        AMBER        GREEN */
    { 0,    H_RED_PIN, H_AMBER_PIN, H_GREEN_PIN },     /* road H */
    { 0,    V_RED_PIN, V_AMBER_PIN, V_GREEN_PIN }      /* road V */
};

static void setRoad(int road, LightColor color)
{
    for (int c = LIGHT_RED; c <= LIGHT_GREEN; c++) {
        HAL_GPIO_WritePin(TL_PORT, ROAD_PIN[road][c],
                          (c == (int)color) ? LED_ON_LEVEL : LED_OFF_LEVEL);
    }
}

/* One colour per road (or LIGHT_OFF); the other colours of that road go dark. */
void setTrafficLeds(LightColor roadH, LightColor roadV)
{
    setRoad(ROAD_H, roadH);
    setRoad(ROAD_V, roadV);
}

/* ------------------------------------------------------------------ */
/*  Init + periodic task                                               */
/* ------------------------------------------------------------------ */
void led_display_init(void)
{
    setTrafficLeds(LIGHT_OFF, LIGHT_OFF);
    HAL_GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, LED_OFF_LEVEL);
    for (int i = 0; i < MAX_LED; i++) {
        HAL_GPIO_WritePin(DIGIT_PORT, DIGIT_PIN[i], DIGIT_OFF_LEVEL);
    }
    index_led = 0;
    setTimer(TIMER_SCAN,   SCAN_PERIOD_MS);
    setTimer(TIMER_STATUS, BLINK_HALF_PERIOD_MS);
}

void led_display_task(void)
{
    /* multiplexing: one digit per SCAN_PERIOD_MS */
    if (isTimerExpired(TIMER_SCAN)) {
        setTimer(TIMER_SCAN, SCAN_PERIOD_MS);
        update7SEG(index_led);
        index_led = (index_led + 1) % MAX_LED;
    }

    /* single LED on PA1 blinks at 2 Hz whatever TIMER_CYCLE is (Exercise 4) */
    if (isTimerExpired(TIMER_STATUS)) {
        setTimer(TIMER_STATUS, BLINK_HALF_PERIOD_MS);
        HAL_GPIO_TogglePin(STATUS_LED_PORT, STATUS_LED_PIN);
    }
}
