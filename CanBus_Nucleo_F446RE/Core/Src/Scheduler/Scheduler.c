/**
 *
 *  Includes
 *
 *
 */

#include "Scheduler.h"


/**
 *
 *  Global Variables
 *
 *
 */

volatile uint32_t tickCounter = 0;
volatile uint8_t fastTaskFlag = 0;
volatile uint8_t mediumTaskFlag = 0;
volatile uint8_t conversionFlag = 0;
volatile uint8_t slowTaskFlag = 0;


/**
 *
 *  TODO A counter is incremented at each each tick until it is not exceeded the saturated value, Otherwise it is reset.
 *  During each tick the counter is compared to the tick time associated at every task: if a task has to be executed, a flag is set (then read into SchedulerMgmFct()
 *  This callback is managed by the Systick_Handler interrupt which is triggered every ms.
 *
 *
 */

void SchTimerInterruptCallback(void)
{
	tickCounter++;

	// Perform quick tasks every 1 ms
	if (tickCounter % 1 == 0) {
		fastTaskFlag = 1;
	}

	// Perform medium tasks every 10 ms
	if (tickCounter % 10 == 0) {
		mediumTaskFlag = 1;
	}

	// Perform the task every 20 ms
	if (tickCounter % 20 == 0) {
		conversionFlag = 1;
	}

	// Perform slow tasks every 100 ms
	if (tickCounter % 100 == 0) {
		slowTaskFlag = 1;
	}

	// Reset the counter to prevent overflow
	if (tickCounter >= 1000) {
		tickCounter = 0;
	}
}


/**
 *
 *  TODO Initialize all the variables and flags associated to the scheduler.
 *  Set the system clock source and the tick (SysTick module) in microseconds
 *
 *
 */

void SchedulerInitFct(void)
{
	// Initialize SysTick to generate an interrupt every 1 ms.
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 160); // Formula for the divider: 128 MHz (of HCLK) / 4 (of APB1 Prescaler) / 2 (of APB2 Prescaler) * 10

	// Initialize other variables and flags if necessary
	tickCounter = 0;
	fastTaskFlag = 0;
	mediumTaskFlag = 0;
	conversionFlag = 0;
	slowTaskFlag = 0;
}


/**
 *
 *  TODO Called by the infinite while loop in the main module.
 *  At each cycle the different flag associated at every task (Fast, Medium, Slow) is checked to determine
 *  which task will run, and the related task function is called if needed.
 *
 *
 */

void SchedulerMgmFct(void)
{
	// Perform quick tasks if the flag is set
	if (fastTaskFlag) {
		fastTaskFlag = 0;
		TaskFast(); // Perform all quick tests
	}

	// Perform medium tasks if the flag is set
	if (mediumTaskFlag) {
		mediumTaskFlag = 0;
		TaskMed(); // Perform all medium-speed tasks
	}

	// Perform average sensors reading conversion before sending to Can
	if (conversionFlag){
		conversionFlag = 0;
		TaskConv(); // Perform the conversion of the average sensors reading
	}

	// Perform slow tasks if the flag is set
	if (slowTaskFlag) {
		slowTaskFlag = 0;
		TaskSlow();  // Perform all slow tasks
	}

	// Other error handling tasks
}

