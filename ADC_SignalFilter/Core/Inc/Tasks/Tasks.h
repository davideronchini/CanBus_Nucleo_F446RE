#ifndef __TASKS_H
#define __TASKS_H

/* ***********************************************************************
 * Required Headers for Module Integration
 * ***********************************************************************
 *
 * This module includes "main.h", which provides essential system-wide
 * configurations and function prototypes. Importantly, including
 * "main.h" also brings in "Scheduler.h", allowing access to scheduling
 * functions and timing mechanisms necessary for effective task management
 * within the system.
 *
 * Ensure that any module utilizing core functionalities or scheduling
 * relies on this header for consistent operation and resource access.
 *********************************************************************** */
#include "main.h"


/* ***********************************************************************
 * Define, Variables, and Functions for Module Export
 * ***********************************************************************
 *
 * This section declares the definitions, variables, and functions that
 * will be visible (exported) to other modules that include "Scheduler.h".
 * These exports facilitate communication and coordination between various
 * tasks and modules within the system, ensuring modularity and clarity
 * in the overall design.
 *
 * The following definitions and variables are provided:
 *
 * - ADC_BUF_LEN: Length of the ADC buffer, set to the number of ADC ranks.
 *
 * The following functions are defined for external access:
 *
 * - TaskInit: Initializes the task scheduling system.
 * - TaskFast: Executes high-priority tasks.
 * - TaskMed: Executes medium-priority tasks.
 * - TaskPeriodicDataTransfer: Executes data transfer operations at a
 *   frequency of 50Hz (0.02 seconds).
 * - TaskSlow: Executes low-priority tasks.
 *
 *********************************************************************** */

#define APPs_ZERO_POSITION 150      // Reference for 0%   -> value measured by the potentiometer
#define APPs_FULL_POSITION 3500     // Reference for 100% -> value measured by the potentiometer
#define APPs_RANGE_INF 0	     	// Reference for 0%   -> value resulting from conversion (mapping)
#define APPs_RANGE_SUP 18600        // Reference for 100% -> value resulting from conversion (mapping)

// The minimum smoothing factor (alpha).
// A smaller value ensures stability in static conditions.
#define ALPHA_BASE 0.0001f

// The adaptation factor for alpha.
// Higher values make the filter more reactive to changes in the signal.
#define ALPHA_ADAPT_FACTOR 0.6f

#define ADC1_BUF_LEN 1
extern uint32_t adc1_buf[ADC1_BUF_LEN]; // Raw readings from each sensor

extern void TaskInit(void);
extern void TaskFast(void);
extern void TaskMed(void);
extern void TaskSlow(void);

extern void Display_Message(UART_HandleTypeDef *huart, const char *format, ...);

void AcquireSensorValues();

#endif /* __TASKS_H */
