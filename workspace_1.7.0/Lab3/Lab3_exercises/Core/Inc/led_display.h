#ifndef INC_LED_DISPLAY_H_
#define INC_LED_DISPLAY_H_

#include "global.h"

#define SCAN_PERIOD_MS      5       /* time per digit: 4 digits -> 50 Hz refresh.
                                       (never faster than one TIMER_CYCLE)      */
#define BLINK_HALF_PERIOD_MS 250    /* 2 Hz blinking = toggle every 250 ms      */

extern int led_buffer[MAX_LED];     /* the 4 digits that are shown (0..9)       */

void led_display_init(void);
void led_display_task(void);        /* scanning + status LED, call in super loop */

/* 7-segment */
void display7SEG(int num);                  /* put one digit on the segment lines */
void update7SEG(int index);                 /* show led_buffer[index] on digit index */
void updateLedBuffer(int left, int right);  /* left -> digits 1-2, right -> digits 3-4 */

/* traffic lights */
void setTrafficLeds(LightColor roadH, LightColor roadV);  /* LIGHT_OFF = all off  */

#endif /* INC_LED_DISPLAY_H_ */
