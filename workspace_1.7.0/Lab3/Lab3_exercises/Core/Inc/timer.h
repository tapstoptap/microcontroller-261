/*
 * timer.h  -  hardware-timer setup + software timers  (Exercise 4)
 *
 * TIMER_CYCLE is the ONLY number that has to change when you want a different
 * TIM2 interrupt period (1 ms, 10 ms, 100 ms ...).  timer_init() derives the
 * prescaler / auto-reload from it, and every software timer is given in
 * milliseconds, so nothing else in the program is affected.
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "main.h"

#ifndef TIMER_CYCLE
#define TIMER_CYCLE     10      /* TIM2 interrupt period in ms (1 .. 6553)   */
#endif

/* Software timer identifiers */
enum {
    TIMER_SCAN = 0,         /* 7-segment scanning                           */
    TIMER_SECOND,           /* 1 s tick of the traffic-light countdown       */
    TIMER_BLINK,            /* 2 Hz blinking in the modify modes             */
    TIMER_STATUS,           /* 2 Hz status LED on PA1                        */
    TIMER_REPEAT_MODE,      /* auto-repeat timers of the three buttons       */
    TIMER_REPEAT_INC,
    TIMER_REPEAT_SET,
    NO_OF_TIMERS
};

void timer_init(void);                      /* configure + start TIM2 (IT)  */
void setTimer(int index, int duration_ms);  /* (re)start a software timer   */
int  isTimerExpired(int index);             /* 1 once the time has elapsed  */
void timer_run(void);                       /* called every TIMER_CYCLE     */

#endif /* INC_TIMER_H_ */
