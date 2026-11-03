/*
 * global.h
 *
 * Pin map, polarity switches and shared types for Lab 3 (Buttons/Switches).
 *
 * The pin map below was read from  Lab3_exercises.pdsprj (Proteus) and
 * Lab3_exercises.ioc (CubeMX).  If your wiring ever changes, this is the ONLY
 * file that has to be edited.
 *
 *   MCU pin        Proteus net     Function
 *   -------------  --------------  ------------------------------------------
 *   PA2            (button 1)      MODE   : select mode 1..4
 *   PA3            (button 2)      INC    : increase the value (1..99)
 *   PA4            (button 3)      SET    : store the value
 *   PB0..PB6       A..G            7SEG-MPX4-CA segments a..g
 *   PA6..PA9       LED1..LED4      7SEG-MPX4-CA digit select (digit 1 = left)
 *   PB8            HGRE            road H  green
 *   PB9            HAMB            road H  amber
 *   PB10           HRED            road H  red
 *   PB11           VGRE            road V  green
 *   PB12           VAMB            road V  amber
 *   PB13           VRED            road V  red
 *   PA1            (single LED)    status LED, blinks at 2 Hz (Exercise 4)
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"
#include <stdint.h>

/* ------------------------------------------------------------------ */
/*  Polarity switches (flip a 1 to 0 if something looks inverted)      */
/* ------------------------------------------------------------------ */
#define BUTTON_ACTIVE_LOW   1   /* schematic: +3.3V --- button --- pin, so pressed = HIGH (needs pull-down) */
#define SEG_ACTIVE_LOW      1   /* common-anode display: 0 = segment lit         */
#define DIGIT_ACTIVE_HIGH   1   /* MPX4-CA digit pin is the anode: 1 = digit on  */
#define LED_ACTIVE_HIGH     1   /* pin --- LED --- R --- GND: 1 = LED on         */

#if BUTTON_ACTIVE_LOW
#define BUTTON_IS_PRESSED   GPIO_PIN_RESET
#define BUTTON_IS_RELEASED  GPIO_PIN_SET
#else
#define BUTTON_IS_PRESSED   GPIO_PIN_SET
#define BUTTON_IS_RELEASED  GPIO_PIN_RESET
#endif

#if SEG_ACTIVE_LOW
#define SEG_ON_LEVEL        GPIO_PIN_RESET
#define SEG_OFF_LEVEL       GPIO_PIN_SET
#else
#define SEG_ON_LEVEL        GPIO_PIN_SET
#define SEG_OFF_LEVEL       GPIO_PIN_RESET
#endif

#if DIGIT_ACTIVE_HIGH
#define DIGIT_ON_LEVEL      GPIO_PIN_SET
#define DIGIT_OFF_LEVEL     GPIO_PIN_RESET
#else
#define DIGIT_ON_LEVEL      GPIO_PIN_RESET
#define DIGIT_OFF_LEVEL     GPIO_PIN_SET
#endif

#if LED_ACTIVE_HIGH
#define LED_ON_LEVEL        GPIO_PIN_SET
#define LED_OFF_LEVEL       GPIO_PIN_RESET
#else
#define LED_ON_LEVEL        GPIO_PIN_RESET
#define LED_OFF_LEVEL       GPIO_PIN_SET
#endif

/* ------------------------------------------------------------------ */
/*  Buttons                                                            */
/* ------------------------------------------------------------------ */
#define NO_OF_BUTTONS       3
enum { BTN_MODE = 0, BTN_INC = 1, BTN_SET = 2 };

#define BTN_MODE_PORT       GPIOA
#define BTN_MODE_PIN        GPIO_PIN_2
#define BTN_INC_PORT        GPIOA
#define BTN_INC_PIN         GPIO_PIN_3
#define BTN_SET_PORT        GPIOA
#define BTN_SET_PIN         GPIO_PIN_4

/* ------------------------------------------------------------------ */
/*  4-digit multiplexed 7-segment display                              */
/* ------------------------------------------------------------------ */
#define MAX_LED             4          /* number of digits                   */

#define SEG_PORT            GPIOB      /* segments a..g = PB0..PB6           */
#define SEG_A_PIN           GPIO_PIN_0
#define SEG_B_PIN           GPIO_PIN_1
#define SEG_C_PIN           GPIO_PIN_2
#define SEG_D_PIN           GPIO_PIN_3
#define SEG_E_PIN           GPIO_PIN_4
#define SEG_F_PIN           GPIO_PIN_5
#define SEG_G_PIN           GPIO_PIN_6

#define DIGIT_PORT          GPIOA      /* digit 1..4 = PA6..PA9 (left..right)*/
#define DIGIT1_PIN          GPIO_PIN_6
#define DIGIT2_PIN          GPIO_PIN_7
#define DIGIT3_PIN          GPIO_PIN_8
#define DIGIT4_PIN          GPIO_PIN_9

/* ------------------------------------------------------------------ */
/*  Traffic lights  (road H = "main road", road V = crossing road)     */
/* ------------------------------------------------------------------ */
#define TL_PORT             GPIOB
#define H_GREEN_PIN         GPIO_PIN_8
#define H_AMBER_PIN         GPIO_PIN_9
#define H_RED_PIN           GPIO_PIN_10
#define V_GREEN_PIN         GPIO_PIN_11
#define V_AMBER_PIN         GPIO_PIN_12
#define V_RED_PIN           GPIO_PIN_13

#define STATUS_LED_PORT     GPIOA
#define STATUS_LED_PIN      GPIO_PIN_1

/* Colours / roads used by the output driver and by the FSM */
typedef enum { LIGHT_OFF = 0, LIGHT_RED, LIGHT_AMBER, LIGHT_GREEN } LightColor;
enum { ROAD_H = 0, ROAD_V = 1 };

/* Operating modes (first button cycles 1 -> 2 -> 3 -> 4 -> 1) */
typedef enum {
    MODE_NORMAL = 1,
    MODE_MODIFY_RED,
    MODE_MODIFY_AMBER,
    MODE_MODIFY_GREEN
} Mode;

#endif /* INC_GLOBAL_H_ */
