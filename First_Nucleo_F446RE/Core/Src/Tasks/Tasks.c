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

#define ADC_BUF_LEN 4096
#define NBR_SENSORS 1


/**
 *
 *  Global Variables
 *
 *
 */

uint16_t adc_buf[ADC_BUF_LEN];

uint16_t raw_readings[NBR_SENSORS];
uint16_t sum_readings[NBR_SENSORS];
uint16_t avg_readings[NBR_SENSORS];


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

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_buf, ADC_BUF_LEN);

	CanInit();
}

void TaskFast(void)
{
	/* TODO functions executed periodically (fast schedulation, e.g. 1 ms). */

	Task1_AcquireSensorValues();
}

void TaskMed(void)
{
	/* TODO functions executed periodically (medium schedulation, e.g. 10 ms). */

	Task1_AverageSensorValues();
}

void TaskSlow(void)
{
	/* TODO functions executed periodically (slow schedulation, e.g. 100 ms). */
	uint8_t a[] = {100, 50, 30};
	//uint8_t g[] = {70, 80, 90};

	Transmit_CAN_Message(&hcan1, 0x033, 3, a);
	//Transmit_CAN_Message(&hcan1, 0x034, 3, g);
}


/**
 *
 *  Specific Tasks
 *
 *
 */
void Task1_AcquireSensorValues(void)
{
	for (uint8_t i = 0; i < hadc1.Init.NbrOfConversion; i++) {
		raw_readings[i] = (uint16_t) adc_buf[i];

		sum_readings[i] += raw_readings[i];

		// Display
		char msg[25];
		sprintf(msg, "%hu) Potentiometer: %hu \r\n", i, raw_readings[i]);
		HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg),
		HAL_MAX_DELAY);
	}
}

void Task1_AverageSensorValues(void)
{
	for (uint8_t i = 0; i < hadc1.Init.NbrOfConversion; i++) {
		avg_readings[i] = sum_readings[i] / 10;
		sum_readings[i] = 0;
	}
}

