#ifndef __SCHEDULER_H
#define __SCHEDULER_H

/**
 * ***********************************************************************
 * SCHEDULER MODULE HEADER
 *
 * Includes all necessary headers and declares variables and functions
 * that are visible to other modules including "Scheduler.h".
 * ***********************************************************************
 */

// Required headers for the Scheduler module
#include "main.h"

#include "Tasks.h"

#include <math.h>

// Standard library inclusion (for serial communication) - to be removed if unused
#include <stdio.h>
#include <stdlib.h>  // Required for NULL and free()
#include <stdarg.h>  // Required for handling variable arguments in printf-style functions

// Exported functions for the Scheduler module
extern void SchTimerInterruptCallback(void);
extern void SchedulerInitFct(void);
extern void SchedulerMgmFct(void);

#endif /* __SCHEDULER_H */


