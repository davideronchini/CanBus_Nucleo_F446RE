#ifndef __SCHEDULER_H
#define __SCHEDULER_H

/* TODO Headers needed to this module (Scheduler) to be included. */
// #include "InterruptDrv.h"
// #include "SysTickDrv.h"
#include "Tasks.h"
#include "Can.h"
#include "main.h"

#include <stdio.h> // TODO: Delete the inclusion of this library if you no longer use the serial


/* TODO Defines, variables and functions to be visible (exported) to other modules that includes "Scheduler.h". */

extern void SchTimerInterruptCallback(void);

extern void SchedulerInitFct(void);

extern uint32_t GetFreeRunningTimerFct(void);

extern void SchedulerMgmFct(void);

#endif /* __SCHEDULER_H */

