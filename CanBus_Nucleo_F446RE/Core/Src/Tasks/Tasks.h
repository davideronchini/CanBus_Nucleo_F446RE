#ifndef __TASKS_H
#define __TASKS_H

/* TODO Headers needed to this module to be included. */
#include "Scheduler.h"
#include "main.h"


/* TODO Defines, variables and functions to be visible (exported) to other modules that includes "Scheduler.h". */

// #define DEBUG_MODE /* Define the Mode of Operation: remove the comment to set debug mode */
#define ADC_BUF_LEN 2

extern uint32_t adc_buf[ADC_BUF_LEN];

extern void TaskInit(void);
extern void TaskFast(void);
extern void TaskMed(void);
extern void TaskConv(void);
extern void TaskSlow(void);

void Task1_AcquireSensorValues(void);
void Task1_AverageSensorValues(void);
void Task2_ConvertAndSendSensorData_Task4_ErrorHandling(void);
void Task3_ReadCANMessages(void);

#endif /* __TASKS_H */

