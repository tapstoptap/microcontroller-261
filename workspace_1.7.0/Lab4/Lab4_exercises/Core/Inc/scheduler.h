#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>

#define SCH_MAX_TASKS 40
#define SCH_TICK_MS 10
#define NO_TASK_ID 0

#define RETURN_ERROR 0
#define RETURN_NORMAL 1

#define ERROR_SCH_TOO_MANY_TASKS 1
#define ERROR_SCH_CANNOT_DELETE_TASK 2
#define ERROR_SCH_INVALID_TASK 3

void SCH_Init(void);
void SCH_Update(void);
void SCH_Dispatch_Tasks(void);
uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD);
uint8_t SCH_Delete_Task(uint32_t taskID);
uint32_t SCH_Get_Ticks(void);
uint8_t SCH_Get_Error(void);
uint32_t get_time(void);

#endif
