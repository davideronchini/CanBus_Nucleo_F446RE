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

uint32_t adc_buf[ADC_BUF_LEN];

uint32_t raw_readings[ADC_BUF_LEN];
uint32_t sum_readings[ADC_BUF_LEN];
uint32_t avg_readings[ADC_BUF_LEN];

// Global variable to track the connection status
volatile uint8_t isConnectionEstablished = 1;

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

	#ifndef DEBUG_MODE
		HAL_ADC_Start_DMA(&hadc1, adc_buf, ADC_BUF_LEN);
	#endif

	CanInit();
}

void TaskFast(void)
{
	/* TODO functions executed periodically (fast schedulation, e.g. 1 ms). */

	#ifndef DEBUG_MODE
		Task1_AcquireSensorValues();
	#endif

	Display_Can_Messages();
}

void TaskMed(void)
{
	/* TODO functions executed periodically (medium schedulation, e.g. 10 ms). */

	#ifndef DEBUG_MODE
		Task1_AverageSensorValues();
	#endif
}

void TaskConv(void)
{
	/* TODO functions executed periodically (medium schedulation, e.g. 20 ms). */

	#ifndef DEBUG_MODE
		Task2_ConvertAndSendSensorData_Task4_ErrorHandling();
	#else
		// Sending accelerometer and gyroscope test values
		uint8_t a[] = {100, 50, 30};
		uint8_t g[] = {70, 80, 90};

		Transmit_CAN_Message(&hcan1, 0x033, 3, a);
		Transmit_CAN_Message(&hcan1, 0x034, 3, g);

	#endif

	// Attention:  since I don't use an interrupt for reception with the MCP2515 can bus transceiver, reception is less accurate.
	Task3_ReadCANMessages();
}

void TaskSlow(void)
{
	/* TODO functions executed periodically (medium schedulation, e.g. 100 ms). */

}


/**
 *
 *  Specific Tasks
 *
 *
 */

/*------------------------------------------------------------------------------
 * Hardware setup description and pin mapping:
 * 2 CAN lines connected:
 *   - CAN1RX -> PD0
 *   - CAN1TX -> PD1
 *   - CAN2RX -> PB6
 *   - CAN2TX -> PB5
 * 5 Sensors with output ranging from 0.3V to 3V connected to pins:
 *   - Sensor 1 -> PA0
 *   - Sensor 2 -> PA4
 *   - Sensor 3 -> PA5
 *   - Sensor 4 -> PA6
 *   - Sensor 5 -> PC3
 *-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * TASK 1
 * Acquire all sensor values at the highest possible speed
 * (read and save readings in memory), then calculate the average every 100
 * readings (for each sensor) and save this data in memory.
 *
 * Suggestions:
 * Use DMA and interrupts to handle reading and averaging.
 *-----------------------------------------------------------------------------*/
void Task1_AcquireSensorValues(void)
{
	for (uint8_t i = 0; i < hadc1.Init.NbrOfConversion; i++) {
		raw_readings[i] = adc_buf[i];

		sum_readings[i] += raw_readings[i];

		// Display
		char msg[50];
		uint16_t msg_length = sprintf(msg, "%u) Potentiometer: %lu\n", i, raw_readings[i]);

		// Transmit message via UART
		HAL_UART_Transmit(&huart2, (uint8_t*)msg, msg_length, HAL_MAX_DELAY);
	}
}

void Task1_AverageSensorValues(void)
{
	for (uint8_t i = 0; i < hadc1.Init.NbrOfConversion; i++) {
		avg_readings[i] = sum_readings[i] / 10;
		sum_readings[i] = 0;
	}
}

/*------------------------------------------------------------------------------
 * TASK 2
 * At a frequency of 50Hz:
 * Convert the data read from the sensors to a scale ranging from 0 to 100 (0.3V corresponds to 0 while
 * 3V corresponds to 100) saving the data as "short int" (2 bytes).
 * Once the data for each sensor is converted, send it on CAN1 with addresses:
 * - 0x035 for sensors 1 to 4
 * - 0x036 for sensor 5
 *
 * Suggestions:
 * Use a timer and interrupts to trigger the procedure, pay attention to the preparation of
 * CAN messages.
 *-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
 * TASK 4:
 * Since the sensors can output values ranging from
 * 0.3V to 3V, if a reading falls outside this range it means the sensor is
 * broken or disconnected. Therefore, if an average reading gives an "out-of-range" value,
 * it is necessary to immediately invoke a function to handle the error.
 *
 * The handling consists of:
 * - Sending an error message on both CAN lines to address 0x005 with
 *   content of 2 bytes: byte 1: 'E' encoded as "char", byte 2: "n" with 'n' being the sensor number
 *   that triggered the error.
 * - Interrupting the transmission of messages from TASK 2 on CAN1 until the
 *   value returns to "normal".
 * - When the value returns to normal, it will be possible to reactivate the board functions
 *   by sending a message on CAN1 to address 0x006 with content
 *   of a single byte containing the letter 'R' encoded as "char".
 *---------------------------------------------------------------------------- */
void Task2_ConvertAndSendSensorData_Task4_ErrorHandling(void)
{
	uint8_t TxData[5] = {0};
	uint8_t allSensorsValid = 1;

	for (uint8_t i = 0; i < hadc1.Init.NbrOfConversion; i++) {
		// Convert the sensor reading to a percentage scale (0% - 100%).
		// The sensor's voltage range is from 0.3V (reading of 373) to 3V (reading of 3724).
		// The maximum possible sensor reading is 4096, corresponding to 110% in this formula.
		// The conversion formula scales the average reading proportionally.
		int8_t conv_reading = (int8_t) (((avg_readings[i] - 373) * 100) / (3723 - 373));

		// Check if the sensor reading is within the valid range
		if (conv_reading >= 0 && conv_reading <= 100){ // OK
		    // Valid reading: update TxData
		    TxData[i] = (uint8_t) conv_reading;
		} else { // ERROR
			// Invalid reading: send error message via CAN
			uint8_t ErrorTxData[2] = {'E', i + 1};
			Transmit_CAN_Message(&hcan1, 0x05, 2, ErrorTxData);
			allSensorsValid = 0;

			isConnectionEstablished = 0; // Mark the connection as unstable
		}
	}

	// If all sensors are valid, send the data via CAN
	if (allSensorsValid) {
		if (!isConnectionEstablished) {
			// Send connection re-establishment message
			uint8_t ReestablishTxData = 'R';
			Transmit_CAN_Message(&hcan1, 0x06, 1, &ReestablishTxData);

			isConnectionEstablished = 1; // Mark the connection as stable
		}

		// Send sensor data via CAN
		Transmit_CAN_Message(&hcan1, 0x35, 4, TxData);
		Transmit_CAN_Message(&hcan1, 0x36, 1, &TxData[4]);
	}
}


/*------------------------------------------------------------------------------
 * TASK 3:
 * Messages arrive on CAN2 with addresses:
 * - 0x033 -> [ax, ay, az]
 * - 0x034 -> [gx, gy, gz]
 * Read these messages and save the data contained in them, taking into account that
 * each variable is encoded as "short int".
 *-----------------------------------------------------------------------------*/
void Task3_ReadCANMessages(void)
{
	#ifndef USE_SN65HVD230
		#ifndef DEBUG_MODE
			Receive_CAN_Message_MCP2515();
		#else
			Debug_Receive_CAN_Message_MCP2515();
		#endif
	#endif
}

