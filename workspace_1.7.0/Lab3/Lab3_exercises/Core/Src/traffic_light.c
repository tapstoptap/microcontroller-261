/*
 * traffic_light.c  -  application FSM (Exercises 5 .. 10)
 *
 *  Mode 1  normal      : 2-road traffic light, 2 digits per road show the
 *                        remaining seconds of the colour that is on.
 *  Mode 2  modify RED  : all 4 red LEDs blink at 2 Hz,
 *  Mode 3  modify AMBER: all 4 amber LEDs blink at 2 Hz,
 *  Mode 4  modify GREEN: all 4 green LEDs blink at 2 Hz.
 *          digits 1-2 show the mode number, digits 3-4 the value (1..99).
 *
 *  Button 1 (MODE): 1 -> 2 -> 3 -> 4 -> 1 ...
 *  Button 2 (INC) : value + 1 (99 wraps to 1); keep it pressed > 1 s and the
 *                   value runs up by itself every 500 ms.
 *  Button 3 (SET) : store the value.  Leaving a mode without SET drops it.
 *
 *  Safety rule: the two roads stay in step only if
 *                   RED = AMBER + GREEN
 *  so storing one of the three durations also adjusts one of the others:
 *     set RED   : GREEN = RED - AMBER           (AMBER shrinks if it must)
 *     set AMBER : GREEN = RED - AMBER           (RED grows if it must)
 *     set GREEN : RED   = GREEN + AMBER
 *  The value shown in the modify mode is refreshed after SET.
 */

#include "main.h"
#include "global.h"
#include "timer.h"
#include "input_processing.h"
#include "led_display.h"
#include "traffic_light.h"

static Mode mode = MODE_NORMAL;

/* durations in seconds */
static int durRed   = DEFAULT_RED_S;
static int durAmber = DEFAULT_AMBER_S;
static int durGreen = DEFAULT_GREEN_S;

/* mode 1 : colour + remaining seconds of each road */
static LightColor colorH, colorV;
static int        cntH, cntV;

/* modes 2..4 : value being edited, blink phase */
static int     tmpValue;
static uint8_t blinkOn;

/* ------------------------------------------------------------------ */
static int durationOf(LightColor c)
{
    switch (c) {
    case LIGHT_RED:   return durRed;
    case LIGHT_AMBER: return durAmber;
    default:          return durGreen;
    }
}

static LightColor colorOfMode(Mode m)
{
    switch (m) {
    case MODE_MODIFY_RED:   return LIGHT_RED;
    case MODE_MODIFY_AMBER: return LIGHT_AMBER;
    default:                return LIGHT_GREEN;
    }
}

/* ------------------------------------------------------------------ */
/*  Mode 1 : normal traffic light                                      */
/* ------------------------------------------------------------------ */
static void showNormal(void)
{
    setTrafficLeds(colorH, colorV);
    updateLedBuffer(cntH, cntV);
}

static void startCycle(void)                 /* road H green, road V red */
{
    colorH = LIGHT_GREEN;  cntH = durGreen;
    colorV = LIGHT_RED;    cntV = durRed;
    setTimer(TIMER_SECOND, 1000);
    showNormal();
}

static void nextColor(LightColor *color, int *cnt)
{
    switch (*color) {
    case LIGHT_GREEN: *color = LIGHT_AMBER; *cnt = durAmber; break;
    case LIGHT_AMBER: *color = LIGHT_RED;   *cnt = durRed;   break;
    default:          *color = LIGHT_GREEN; *cnt = durGreen; break;
    }
}

static void trafficTick(void)                /* once per second */
{
    if (--cntH <= 0) nextColor(&colorH, &cntH);
    if (--cntV <= 0) nextColor(&colorV, &cntV);
    showNormal();
}

/* ------------------------------------------------------------------ */
/*  Modes 2..4 : modify a duration                                     */
/* ------------------------------------------------------------------ */
static void showModify(void)
{
    LightColor c = blinkOn ? colorOfMode(mode) : LIGHT_OFF;
    setTrafficLeds(c, c);                    /* all 4 LEDs of that colour */
    updateLedBuffer((int)mode, tmpValue);
}

static void commitValue(void)
{
    int v = tmpValue;

    switch (mode) {
    case MODE_MODIFY_RED:
        if (v < 2) v = 2;                              /* amber >= 1 and green >= 1 */
        durRed   = v;
        durGreen = durRed - durAmber;
        if (durGreen < 1) { durGreen = 1; durAmber = durRed - 1; }
        break;

    case MODE_MODIFY_AMBER:
        if (v > 98) v = 98;                            /* green >= 1, red <= 99     */
        durAmber = v;
        durGreen = durRed - durAmber;
        if (durGreen < 1) { durGreen = 1; durRed = durAmber + 1; }
        break;

    case MODE_MODIFY_GREEN:
        if (v > 99 - durAmber) v = 99 - durAmber;      /* red <= 99                 */
        durGreen = v;
        durRed   = durGreen + durAmber;
        break;

    default:
        return;
    }
    tmpValue = durationOf(colorOfMode(mode));          /* show what was really stored */
}

/* ------------------------------------------------------------------ */
/*  Mode switching                                                     */
/* ------------------------------------------------------------------ */
static void enterMode(Mode m)
{
    mode = m;
    if (m == MODE_NORMAL) {
        startCycle();                        /* restart with the (new) durations */
    } else {
        tmpValue = durationOf(colorOfMode(m));
        blinkOn  = 1;
        setTimer(TIMER_BLINK, BLINK_HALF_PERIOD_MS);
        showModify();
    }
}

void traffic_light_init(void)
{
    durRed   = DEFAULT_RED_S;
    durAmber = DEFAULT_AMBER_S;
    durGreen = DEFAULT_GREEN_S;
    enterMode(MODE_NORMAL);
}

/* ------------------------------------------------------------------ */
/*  Super-loop task                                                    */
/* ------------------------------------------------------------------ */
void fsm_traffic_light(void)
{
    /* button 1 : next mode (1 -> 2 -> 3 -> 4 -> 1) */
    if (button_clicked(BTN_MODE)) {
        enterMode((mode == MODE_MODIFY_GREEN) ? MODE_NORMAL : (Mode)(mode + 1));
    }
    (void)button_repeated(BTN_MODE);         /* not used: drop stale events */
    (void)button_repeated(BTN_SET);

    if (mode == MODE_NORMAL) {
        (void)button_clicked(BTN_INC);       /* buttons 2 and 3 do nothing here */
        (void)button_repeated(BTN_INC);
        (void)button_clicked(BTN_SET);

        if (isTimerExpired(TIMER_SECOND)) {
            setTimer(TIMER_SECOND, 1000);
            trafficTick();
        }
    } else {
        int inc = button_clicked(BTN_INC);
        inc |= button_repeated(BTN_INC);
        if (inc) {
            tmpValue = (tmpValue >= 99) ? 1 : tmpValue + 1;      /* range 1..99 */
            showModify();
        }

        if (button_clicked(BTN_SET)) {
            commitValue();
            showModify();
        }

        if (isTimerExpired(TIMER_BLINK)) {
            setTimer(TIMER_BLINK, BLINK_HALF_PERIOD_MS);
            blinkOn ^= 1U;                   /* 2 Hz */
            showModify();
        }
    }
}

/* ------------------------------------------------------------------ */
Mode traffic_get_mode(void)            { return mode; }
int  traffic_get_duration(LightColor c) { return durationOf(c); }
