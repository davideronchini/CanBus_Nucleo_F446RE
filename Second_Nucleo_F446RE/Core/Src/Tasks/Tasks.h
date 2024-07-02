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

void Task3_ReadCANMessages(void);

#endif /* __TASKS_H */

