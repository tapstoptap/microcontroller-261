/*
 * input_reading.c  -  button reading + debouncing  (Exercise 5)
 *
 * Same technique as the manual (Program 1.2): two consecutive samples must be
 * equal before the debounced value buttonBuffer[] is updated (N = 2).
 *
 * Differences from the manual's listing:
 *   - three buttons instead of one (pins come from global.h),
 *   - the buffers start in the RELEASED state (a static array is all zeros,
 *     which would otherwise mean "pressed" and create a phantom key press
 *     right after reset),
 *   - sampling happens every BUTTON_SCAN_MS regardless of TIMER_CYCLE.
 */

#include "main.h"
#include "global.h"
#include "timer.h"
#include "input_reading.h"

#define BUTTON_SCAN_MS          10      /* > bounce time (10 ms), < 50 ms response */

/* interrupts between two samples (>= 1) and the real sampling period */
#define BUTTON_SCAN_TICKS       (((BUTTON_SCAN_MS / TIMER_CYCLE) > 0) ? (BUTTON_SCAN_MS / TIMER_CYCLE) : 1)
#define BUTTON_SCAN_PERIOD_MS   (BUTTON_SCAN_TICKS * TIMER_CYCLE)

/* a button counts as "long pressed" after 1 s = this many samples */
#define DURATION_FOR_AUTO_INCREASING   (1000 / BUTTON_SCAN_PERIOD_MS)

static GPIO_TypeDef * const buttonPort[NO_OF_BUTTONS] = { BTN_MODE_PORT, BTN_INC_PORT, BTN_SET_PORT };
static const uint16_t       buttonPin [NO_OF_BUTTONS] = { BTN_MODE_PIN,  BTN_INC_PIN,  BTN_SET_PIN  };

/* final result after debouncing */
static GPIO_PinState buttonBuffer[NO_OF_BUTTONS];
/* two buffers for debouncing */
static GPIO_PinState debounceButtonBuffer1[NO_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffer2[NO_OF_BUTTONS];
/* flag + counter for "pressed for more than 1 second" */
static uint8_t  flagForButtonPress1s[NO_OF_BUTTONS];
static uint16_t counterForButtonPress1s[NO_OF_BUTTONS];

void button_init(void)
{
    /* Inputs with the internal pull-up enabled (the schematic has only a
     * button to GND, no external resistor). */
    GPIO_InitTypeDef gpio = {0};
    for (int i = 0; i < NO_OF_BUTTONS; i++) {
        gpio.Pin  = buttonPin[i];
        gpio.Mode = GPIO_MODE_INPUT;
#if BUTTON_ACTIVE_LOW
        gpio.Pull = GPIO_PULLUP;
#else
        gpio.Pull = GPIO_PULLDOWN;
#endif
        HAL_GPIO_Init(buttonPort[i], &gpio);

        buttonBuffer[i]          = BUTTON_IS_RELEASED;
        debounceButtonBuffer1[i] = BUTTON_IS_RELEASED;
        debounceButtonBuffer2[i] = BUTTON_IS_RELEASED;
        flagForButtonPress1s[i]    = 0;
        counterForButtonPress1s[i] = 0;
    }
}

/* Called from the timer interrupt every TIMER_CYCLE ms. */
void button_reading(void)
{
    static uint8_t tick = 0;
    if (++tick < BUTTON_SCAN_TICKS) return;
    tick = 0;

    for (int i = 0; i < NO_OF_BUTTONS; i++) {
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
        debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(buttonPort[i], buttonPin[i]);

        if (debounceButtonBuffer1[i] == debounceButtonBuffer2[i]) {
            buttonBuffer[i] = debounceButtonBuffer1[i];
        }

        if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
            /* if a button is pressed, we start counting */
            if (counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING) {
                counterForButtonPress1s[i]++;
            } else {
                /* the flag is turned on when 1 second has passed
                 * since the button is pressed. */
                flagForButtonPress1s[i] = 1;
            }
        } else {
            counterForButtonPress1s[i] = 0;
            flagForButtonPress1s[i]    = 0;
        }
    }
}

unsigned char is_button_pressed(unsigned char index)
{
    if (index >= NO_OF_BUTTONS) return 0;
    return (buttonBuffer[index] == BUTTON_IS_PRESSED);
}

unsigned char is_button_pressed_1s(unsigned char index)
{
    if (index >= NO_OF_BUTTONS) return 0;
    return (flagForButtonPress1s[index] == 1);
}
