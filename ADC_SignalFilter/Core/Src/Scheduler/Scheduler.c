/* ***********************************************************************
 * Includes
 * ***********************************************************************/

#include "Scheduler.h"


/* ***********************************************************************
 * Private Defines
 * ***********************************************************************/


/* ***********************************************************************
 * Global Variables
 * ***********************************************************************/

volatile uint32_t tickCounter = 0;
volatile uint8_t fastTaskFlag = 0;
volatile uint8_t mediumTaskFlag = 0;
volatile uint8_t slowTaskFlag = 0;


/**
 * ************************************************************************
 * TIMER INTERRUPT CALLBACK - Increment tick counter and set task flags
 * based on elapsed intervals.
 *
 * This function is called by the SysTick_Handler interrupt every 1 ms.
 * It increments a counter (tickCounter) and sets flags for specific
 * tasks at predefined intervals:
 *
 * - Fast task flag set every 1 ms
 * - Medium task flag set every 10 ms
 * - Conversion task flag set every 20 ms
 * - Slow task flag set every 100 ms
 *
 * When tickCounter reaches 1000 ms, it is reset to avoid overflow.
 * ************************************************************************
 */

void SchTimerInterruptCallback(void)
{
	tickCounter++;

	// Set flags for tasks based on predefined intervals
	if (tickCounter % 1 == 0)   fastTaskFlag = 1;      // Fast tasks every 1 ms
	if (tickCounter % 10 == 0)  mediumTaskFlag = 1;    // Medium tasks every 10 ms
	if (tickCounter % 100 == 0) slowTaskFlag = 1;      // Slow tasks every 100 ms

	// Reset counter to prevent overflow after full cycle
	if (tickCounter >= 100) tickCounter = 0;
}


/**
 * ************************************************************************
 * SCHEDULER INITIALIZATION FUNCTION - Configures system clock and
 * initializes scheduler flags and counters.
 *
 * This function sets up SysTick to generate an interrupt every 1 ms
 * (using HAL_SYSTICK_Config) and resets all task flags and counters
 * associated with the scheduler.
 * ************************************************************************
 */

void SchedulerInitFct(void)
{
	/**
	 * ***********************************************************************
	 * CONFIGURE SYSTICK FOR 1 MS INTERRUPT INTERVAL
	 *
	 * Sets up SysTick to trigger an interrupt every 1 ms.
	 * Formula for the divider: 128 MHz (HCLK) / 4 (APB1 Prescaler) / 2 (APB2 Prescaler) * 10
	 * ***********************************************************************
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 160);

	// Initialize scheduler flags and counters
	tickCounter = 0;

	fastTaskFlag = 0;
	mediumTaskFlag = 0;
	slowTaskFlag = 0;
}


/**
 * ***********************************************************************
 * SCHEDULER MANAGEMENT FUNCTION
 *
 * Called by the main module's infinite loop. This function checks each
 * task flag (Fast, Medium, Periodic, Slow) to determine which task should
 * execute, and calls the corresponding task function if the flag is set.
 * ***********************************************************************
 */
void SchedulerMgmFct(void)
{
    // Execute fast tasks if the flag is set
    if (fastTaskFlag) {
        fastTaskFlag = 0;
        TaskFast(); // Execute all high-priority tasks
    }

    // Execute medium tasks if the flag is set
    if (mediumTaskFlag) {
        mediumTaskFlag = 0;
        TaskMed(); // Execute all medium-priority tasks
    }

    // Execute slow tasks if the flag is set
    if (slowTaskFlag) {
        slowTaskFlag = 0;
        TaskSlow(); // Execute all low-priority tasks
    }

    // Additional error handling tasks (if necessary)
}


