/*
 * input_processing.c  -  button state machine (manual, Figure 1.9)
 *
 *   RELEASED --(pressed)--------------> PRESSED   + one "click" event
 *   PRESSED  --(released)-------------> RELEASED
 *   PRESSED  --(held > 1 s)-----------> PRESSED_MORE_THAN_1_SECOND
 *   PRESSED_MORE_THAN_1_SECOND: one "repeat" event every AUTO_REPEAT_MS
 *   PRESSED_MORE_THAN_1_SECOND --(released)--> RELEASED
 *
 * One FSM instance per button.
 */

#include "main.h"
#include "global.h"
#include "timer.h"
#include "input_reading.h"
#include "input_processing.h"

typedef enum {
    BUTTON_RELEASED,
    BUTTON_PRESSED,
    BUTTON_PRESSED_MORE_THAN_1_SECOND
} ButtonState;

static ButtonState buttonState[NO_OF_BUTTONS];          /* all BUTTON_RELEASED */
static uint8_t     clickFlag [NO_OF_BUTTONS];
static uint8_t     repeatFlag[NO_OF_BUTTONS];

void fsm_for_input_processing(void)
{
    for (unsigned char i = 0; i < NO_OF_BUTTONS; i++) {
        switch (buttonState[i]) {

        case BUTTON_RELEASED:
            if (is_button_pressed(i)) {
                buttonState[i] = BUTTON_PRESSED;
                clickFlag[i]   = 1;                     /* act once per press */
            }
            break;

        case BUTTON_PRESSED:
            if (!is_button_pressed(i)) {
                buttonState[i] = BUTTON_RELEASED;
            } else if (is_button_pressed_1s(i)) {
                buttonState[i] = BUTTON_PRESSED_MORE_THAN_1_SECOND;
                setTimer(TIMER_REPEAT_MODE + i, AUTO_REPEAT_MS);
            }
            break;

        case BUTTON_PRESSED_MORE_THAN_1_SECOND:
            if (!is_button_pressed(i)) {
                buttonState[i] = BUTTON_RELEASED;
            } else if (isTimerExpired(TIMER_REPEAT_MODE + i)) {
                repeatFlag[i] = 1;                      /* auto-repeat tick   */
                setTimer(TIMER_REPEAT_MODE + i, AUTO_REPEAT_MS);
            }
            break;
        }
    }
}

unsigned char button_clicked(unsigned char index)
{
    if (index >= NO_OF_BUTTONS || !clickFlag[index]) return 0;
    clickFlag[index] = 0;
    return 1;
}

unsigned char button_repeated(unsigned char index)
{
    if (index >= NO_OF_BUTTONS || !repeatFlag[index]) return 0;
    repeatFlag[index] = 0;
    return 1;
}
