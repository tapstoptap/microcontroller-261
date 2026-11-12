#include "scheduler.h"
#include "main.h"

#if (SCH_MAX_TASKS < 1) || (SCH_MAX_TASKS > 254)
#error "SCH_MAX_TASKS must be between 1 and 254"
#endif

#define SCH_NIL 0xFFu

typedef struct {
    void (*pTask)(void);
    uint32_t Delay;
    uint32_t Period;
    uint32_t Due;
    uint32_t TaskID;
    uint8_t Next;
} sTask;

static sTask SCH_tasks_G[SCH_MAX_TASKS];
static uint8_t head_G = SCH_NIL;
static uint8_t free_G = SCH_NIL;
static volatile uint32_t ticks_G = 0;
static uint32_t backlog_G = 0;
static uint32_t lastID_G = NO_TASK_ID;
static uint8_t Error_code_G = 0;

static uint32_t sch_lock(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static void sch_unlock(uint32_t primask)
{
    __set_PRIMASK(primask);
}

static void sch_catch_up(void)
{
    uint8_t cur = head_G;
    while ((backlog_G != 0u) && (cur != SCH_NIL)) {
        if (SCH_tasks_G[cur].Delay >= backlog_G) {
            SCH_tasks_G[cur].Delay -= backlog_G;
            backlog_G = 0u;
        } else {
            backlog_G -= SCH_tasks_G[cur].Delay;
            SCH_tasks_G[cur].Delay = 0u;
            cur = SCH_tasks_G[cur].Next;
        }
    }
    backlog_G = 0u;
}

static void sch_link(uint8_t index, uint32_t delay)
{
    uint8_t prev = SCH_NIL;
    uint8_t cur = head_G;
    while (cur != SCH_NIL) {
        uint32_t d = SCH_tasks_G[cur].Delay;
        if (delay < d) {
            break;
        }
        if ((delay == d) && ((int32_t)(SCH_tasks_G[cur].Due - SCH_tasks_G[index].Due) > 0)) {
            break;
        }
        delay -= d;
        prev = cur;
        cur = SCH_tasks_G[cur].Next;
    }
    SCH_tasks_G[index].Delay = delay;
    SCH_tasks_G[index].Next = cur;
    if (cur != SCH_NIL) {
        SCH_tasks_G[cur].Delay -= delay;
    }
    if (prev == SCH_NIL) {
        head_G = index;
    } else {
        SCH_tasks_G[prev].Next = index;
    }
}

static void sch_free(uint8_t index)
{
    SCH_tasks_G[index].pTask = 0;
    SCH_tasks_G[index].Delay = 0u;
    SCH_tasks_G[index].Period = 0u;
    SCH_tasks_G[index].Due = 0u;
    SCH_tasks_G[index].TaskID = NO_TASK_ID;
    SCH_tasks_G[index].Next = free_G;
    free_G = index;
}

static uint32_t sch_new_id(void)
{
    uint8_t cur;
    do {
        lastID_G++;
        if (lastID_G == NO_TASK_ID) {
            lastID_G++;
        }
        cur = head_G;
        while ((cur != SCH_NIL) && (SCH_tasks_G[cur].TaskID != lastID_G)) {
            cur = SCH_tasks_G[cur].Next;
        }
    } while (cur != SCH_NIL);
    return lastID_G;
}

void SCH_Init(void)
{
    uint32_t primask = sch_lock();
    uint8_t i;
    head_G = SCH_NIL;
    free_G = SCH_NIL;
    for (i = SCH_MAX_TASKS; i > 0u; i--) {
        sch_free((uint8_t)(i - 1u));
    }
    ticks_G = 0u;
    backlog_G = 0u;
    lastID_G = NO_TASK_ID;
    Error_code_G = 0u;
    sch_unlock(primask);
}

void SCH_Update(void)
{
    ticks_G++;
    if (head_G != SCH_NIL) {
        if (SCH_tasks_G[head_G].Delay != 0u) {
            SCH_tasks_G[head_G].Delay--;
        } else {
            backlog_G++;
        }
    }
}

uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD)
{
    uint32_t primask;
    uint32_t id;
    uint8_t index;
    if (pFunction == 0) {
        Error_code_G = ERROR_SCH_INVALID_TASK;
        return NO_TASK_ID;
    }
    primask = sch_lock();
    index = free_G;
    if (index == SCH_NIL) {
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        sch_unlock(primask);
        return NO_TASK_ID;
    }
    free_G = SCH_tasks_G[index].Next;
    sch_catch_up();
    id = sch_new_id();
    SCH_tasks_G[index].pTask = pFunction;
    SCH_tasks_G[index].Period = PERIOD;
    SCH_tasks_G[index].Due = ticks_G + DELAY;
    SCH_tasks_G[index].TaskID = id;
    sch_link(index, DELAY);
    sch_unlock(primask);
    return id;
}

uint8_t SCH_Delete_Task(uint32_t taskID)
{
    uint32_t primask;
    uint8_t prev = SCH_NIL;
    uint8_t cur;
    uint8_t next;
    if (taskID == NO_TASK_ID) {
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
        return RETURN_ERROR;
    }
    primask = sch_lock();
    sch_catch_up();
    cur = head_G;
    while ((cur != SCH_NIL) && (SCH_tasks_G[cur].TaskID != taskID)) {
        prev = cur;
        cur = SCH_tasks_G[cur].Next;
    }
    if (cur == SCH_NIL) {
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
        sch_unlock(primask);
        return RETURN_ERROR;
    }
    next = SCH_tasks_G[cur].Next;
    if (next != SCH_NIL) {
        SCH_tasks_G[next].Delay += SCH_tasks_G[cur].Delay;
    }
    if (prev == SCH_NIL) {
        head_G = next;
    } else {
        SCH_tasks_G[prev].Next = next;
    }
    sch_free(cur);
    sch_unlock(primask);
    return RETURN_NORMAL;
}

static void SCH_Go_To_Sleep(void)
{
    __WFI();
}

void SCH_Dispatch_Tasks(void)
{
    for (;;) {
        void (*task)(void);
        uint32_t primask = sch_lock();
        uint8_t index;
        sch_catch_up();
        index = head_G;
        if ((index == SCH_NIL) || (SCH_tasks_G[index].Delay != 0u)) {
            sch_unlock(primask);
            SCH_Go_To_Sleep();
            return;
        }
        head_G = SCH_tasks_G[index].Next;
        task = SCH_tasks_G[index].pTask;
        if (SCH_tasks_G[index].Period != 0u) {
            uint32_t late = ticks_G - SCH_tasks_G[index].Due;
            uint32_t period = SCH_tasks_G[index].Period;
            SCH_tasks_G[index].Due += period;
            sch_link(index, (late >= period) ? 0u : (period - late));
        } else {
            sch_free(index);
        }
        sch_unlock(primask);
        task();
    }
}

uint32_t SCH_Get_Ticks(void)
{
    return ticks_G;
}

uint8_t SCH_Get_Error(void)
{
    return Error_code_G;
}

uint32_t get_time(void)
{
    return ticks_G * SCH_TICK_MS;
}
