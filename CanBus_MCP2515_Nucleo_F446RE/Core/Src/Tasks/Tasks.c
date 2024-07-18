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
 *  Define the Mode of Operation:
 *   - remove the comment to set the desired mode -
 *
 *
 */

// #define DEBUG_MODE


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
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_buf, ADC_BUF_LEN);
	#endif

	CanInit();
}

void TaskFast(void)
{
	/* TODO functions executed periodically (fast schedulation, e.g. 1 ms). */

	#ifndef DEBUG_MODE
		Task1_AcquireSensorValues();
	#endif
}

void TaskMed(void)
{
	/* TODO functions executed periodically (medium schedulation, e.g. 10 ms). */

	#ifndef DEBUG_MODE
		Task1_AverageSensorValues();

		Task3_ReadCANMessages();
	#endif
}

void TaskConv(void)
{
	/* TODO functions executed periodically (medium schedulation, e.g. 20 ms). */

	#ifndef DEBUG_MODE
		Task2_ConvertAndSendSensorData_Task4_ErrorHandling();
	#endif
}

void TaskSlow(void)
{
	/* TODO functions executed periodically (medium schedulation, e.g. 100 ms). */

	#ifdef DEBUG_MODE
		Debug_CAN_Messages();
	#endif
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
		raw_readings[i] = (uint16_t) adc_buf[i];

		sum_readings[i] += raw_readings[i];

		// Display
		char msg[] = ") Potentiometer: ";
		HAL_UART_Transmit(&huart2, (uint8_t*) &i, 1, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*) msg, sizeof(msg) - 1, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*) &raw_readings[i], 1, HAL_MAX_DELAY);

		uint8_t lf = '\n';
		HAL_UART_Transmit(&huart2, &lf, 1, HAL_MAX_DELAY);
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
	uint8_t resetFlag = 0;

	uint8_t TxData[5];
	for(uint8_t i = 0; i < 5; i++){
		TxData[i] = 0;
	}

	int8_t conv_readings[NBR_SENSORS];
	for (uint8_t i = 0; i < hadc1.Init.NbrOfConversion; i++) {
		// Convert the sensor reading to a percentage scale (0% - 100%).
		// The sensor's voltage range is from 0.3V (reading of 373) to 3V (reading of 3724).
		// The maximum possible sensor reading is 4096, corresponding to 110% in this formula.
		// The conversion formula scales the average reading proportionally.
		conv_readings[i] = (int8_t)(((avg_readings[i]-373) * 100) / (3723 - 373));

		// Display
		/*char msg[50];
		sprintf(msg, "\n\n%hu) Converted Values: %d \r\n", i, conv_readings[i]);
		HAL_UART_Transmit(&huart3, (uint8_t*) msg, strlen(msg),
		HAL_MAX_DELAY);*/

		if (conv_readings[i] >= 0 && conv_readings[i] <= 100){ // OK

			//Re-establish the connection
			if(resetFlag){
				resetFlag = 0;

				// Assign the connection re-establishment message to the CAN transmit data
				uint8_t ReestablishTxData = (uint8_t)'R';
				Transmit_CAN_Message(0x06, 1, &ReestablishTxData); // Add CAN message to transmit queue
			}

			// Assign the converted reading to the CAN transmit data
			TxData[i] = (uint8_t) conv_readings[i];
		}else{ // ERROR
			resetFlag = 1;

			// Assign the error message to the CAN transmit data
			uint8_t ErrorTxData[2] = {(uint8_t) 'E', i + 1};
			Transmit_CAN_Message(0x05, 2, ErrorTxData); // Add CAN message to transmit queue
			// TODO: enable this code if can2 is enabled:
			//Transmit_CAN_Message(&hcan2, 0x005, 2, &TxData);
		}
	}

	if (!resetFlag){
		// Send sensor data via CAN
		Transmit_CAN_Message(0x35, 4, TxData);
		Transmit_CAN_Message(0x36, 1, &TxData[4]);
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
	Receive_CAN_Message();
}

void Debug_CAN_Messages(void)
{
	// Sending accelerometer and gyroscope test values
	uint8_t a[] = {100, 50, 30};
	uint8_t g[] = {70, 80, 90};

	Transmit_CAN_Message(0x033, 3, a);
	Transmit_CAN_Message(0x034, 3, g);

	// Attempt to receive a CAN message
	uCAN_MSG rxMessage;
	if (CANSPI_Receive(&rxMessage)) {

		// Character for carriage return in messages sent via serial
		uint8_t lf = '\n';

		// Check for specific CAN message ID
		switch(rxMessage.frame.id){
			case 0x35:
				// Extract data from CAN message
				uint8_t s1 = (uint8_t) rxMessage.frame.data0;
				uint8_t s2 = (uint8_t) rxMessage.frame.data1;
				uint8_t s3 = (uint8_t) rxMessage.frame.data2;
				uint8_t s4 = (uint8_t) rxMessage.frame.data3;

				// Display received messages
				const char msg_35_1[] = "Sensor 1: ";
				HAL_UART_Transmit(&huart2, (uint8_t*) msg_35_1, sizeof(msg_35_1) - 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, (uint8_t*) &s1, 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, &lf, 1, HAL_MAX_DELAY);

				const char msg_35_2[] = "Sensor 2: ";
				HAL_UART_Transmit(&huart2, (uint8_t*) msg_35_2, sizeof(msg_35_2) - 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, (uint8_t*) &s2, 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, &lf, 1, HAL_MAX_DELAY);

				const char msg_35_3[] = "Sensor 3: ";
				HAL_UART_Transmit(&huart2, (uint8_t*) msg_35_3, sizeof(msg_35_3) - 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, (uint8_t*) &s3, 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, &lf, 1, HAL_MAX_DELAY);

				const char msg_35_4[] = "Sensor 4: ";
				HAL_UART_Transmit(&huart2, (uint8_t*) msg_35_4, sizeof(msg_35_4) - 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, (uint8_t*) &s4, 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, &lf, 1, HAL_MAX_DELAY);

				break;

			case 0x36:
				// Extract data from CAN message
				uint8_t s5 = (uint8_t) rxMessage.frame.data0;

				// Display received messages
				const char msg_36[] = "Sensor 5: ";
				HAL_UART_Transmit(&huart2, (uint8_t*) msg_36, sizeof(msg_36) - 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, (uint8_t*) &s5, 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, &lf, 1, HAL_MAX_DELAY);

				break;

			case 0x05:
				// Extract data from CAN message
				char eChar = (char) rxMessage.frame.data0;
				uint8_t sensorNumber = (uint8_t) rxMessage.frame.data1;

				// Display received messages
				const char msg_05[] = "Error: ";
				HAL_UART_Transmit(&huart2, (uint8_t*) msg_05, sizeof(msg_05) - 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, (uint8_t*) &eChar, 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, (uint8_t*) &sensorNumber, 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, &lf, 1, HAL_MAX_DELAY);

				break;

			case 0x06:
				// Extract data from CAN message
				char receivedChar = (char) rxMessage.frame.data0;

				// Display received messages
				const char msg_06[] = "Connection Reestablished: ";
				HAL_UART_Transmit(&huart2, (uint8_t*) msg_06, sizeof(msg_06) - 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, (uint8_t*) &receivedChar, 1, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart2, &lf, 1, HAL_MAX_DELAY);

				break;
		}

		// Indicate successful reception by turning on the LED
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	} else {
		// Indicate reception error by turning off the LED
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	}
}

