#ifndef __TASKS_H
#define __TASKS_H

/* TODO Headers needed to this module to be included. */
#include "Scheduler.h"
#include "main.h"


/* TODO Defines, variables and functions to be visible (exported) to other modules that includes "Scheduler.h". */
extern void TaskInit(void);
extern void TaskFast(void);
extern void TaskMed(void);
extern void TaskSlow(void);

void Task1_AcquireSensorValues(void);
void Task1_AverageSensorValues(void);
void Task2_ConvertAndSendSensorData(void);
void Task3_ReadCANMessages(void);
void Task4_ErrorHandling(void);

#endif /* __TASKS_H */

