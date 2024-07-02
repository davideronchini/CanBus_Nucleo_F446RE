/**
 *
 *  Includes
 *
 *
 */

#include "Tasks.h"


/**
 *
 *  Private Defines
 *
 *
 */


/**
 *
 *  Global Variables
 *
 *
 */


/**
 *
 *  General Tasks
 *
 *
 */
void TaskInit(void)
{
	/* Executed once at startup. */
	/* TODO Functions for HW/SW initialization. */

	SchedulerInitFct();
	CanInit();
}

void TaskFast(void)
{
	/* TODO functions executed periodically (fast schedulation, e.g. 1 ms). */
}

void TaskMed(void)
{
	/* TODO functions executed periodically (medium schedulation, e.g. 10 ms). */

}

void TaskSlow(void)
{
	/* TODO functions executed periodically (slow schedulation, e.g. 100 ms). */

}


/**
 *
 *  Specific Tasks
 *
 *
 */

