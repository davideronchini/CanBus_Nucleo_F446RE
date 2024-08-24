/**
 *
 *  Includes
 *
 *
 */
#include "Can.h"


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
uCAN_MSG txMessage;
uCAN_MSG rxMessage;

Vector a = {0};
Vector g = {0};

typedef struct{
    volatile uint8_t msg0x33;
    volatile uint8_t msg0x34;
    volatile uint8_t msg0x35;
    volatile uint8_t msg0x36;
    volatile uint8_t msg0x05;
    volatile uint8_t msg0x06;
} CAN_Flags;

CAN_Flags can_flags;

uint8_t s1, s2, s3, s4, s5;

char eChar;
uint8_t sensorNumber;

char receivedChar;


/**
 *
 *  Functions
 *
 *
 */
void CanInit(void)
{
	/* Executed once at startup. */

	// Configuring CAN filter parameters
	CAN_FilterTypeDef sFilterConfig;

	// Use filter bank 0 for this filter configuration
	// Filter banks are memory locations where filter settings are stored.
	// The STM32F466RE has a maximum of 28 filter banks.
	// Each filter bank can hold one or more filters. Bank 0 is the first of these banks.
	sFilterConfig.FilterBank = 0;

	// Set the filter to operate in identifier mask mode
	// This mode allows filtering based on specific bits in the CAN ID.
	// The filter ID and mask determine which CAN messages are accepted.
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;

	// Set the filter to 32-bit scale
	// This means the filter will use a single 32-bit filter instead of two 16-bit filters.
	// A 32-bit filter can match a full 29-bit extended CAN ID or two 11-bit standard CAN IDs.
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;

	// Set the high part of the filter ID to 0x0000
	// This is the upper 16 bits of the 32-bit filter ID.
	// It is used to match the incoming CAN messages.
	sFilterConfig.FilterIdHigh = 0x0000;

	// Set the low part of the filter ID to 0x0000
	// This is the lower 16 bits of the 32-bit filter ID.
	// Together with FilterIdHigh, it forms the full 32-bit ID to match incoming messages.
	sFilterConfig.FilterIdLow = 0x0000;

	// Set the high part of the filter mask to 0x0000
	// This is the upper 16 bits of the mask used to determine which bits in the ID are significant.
	// A mask bit set to 0 means the corresponding ID bit is "don't care".
	sFilterConfig.FilterMaskIdHigh = 0x0000;

	// Set the low part of the filter mask to 0x0000
	// This is the lower 16 bits of the mask.
	// Together with FilterMaskIdHigh, it forms the full 32-bit mask.
	// A mask of 0x0000 means all bits are "don't care" and will accept all IDs.
	sFilterConfig.FilterMaskIdLow = 0x0000;

	// Assign this filter to CAN receive FIFO 0
	// Incoming messages that pass this filter will be stored in FIFO 0.
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;

	// Enable this filter configuration
	// Activates the filter so that it starts processing incoming messages.
	sFilterConfig.FilterActivation = ENABLE;

	// Configure the start filter bank for the slave CAN controller (CAN2) to filter bank 14.
	// The STM32F4 microcontroller has a total of 28 filter banks available for CAN message filtering.
	// These filter banks can be shared between the two CAN controllers: CAN1 (master) and CAN2 (slave).
	// By setting sFilterConfig.SlaveStartFilterBank to 14, the first 14 filter banks (0 to 13) are allocated to CAN1,
	// and the remaining 14 filter banks (14 to 27) are allocated to CAN2.
	// This division allows each CAN controller to have its own set of filters for incoming CAN messages.
	// Adjusting this value allows for flexible distribution of the available filter banks based on the application's requirements.
	sFilterConfig.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
	{
		// Gestione errore
		Error_Handler();
	}

	// Initialize the SN65HVD230 can transceiver
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); // This interrupt is triggered when there is at least one new message pending in FIFO 0

	// Initialize the MCP2515 Can Transceiver
	int result = CANSPI_Initialize();
	if(result < 0)
	{
		// TODO: Handle the error
	}
}

void Transmit_CAN_Message(CAN_HandleTypeDef *hcan, uint32_t StdId, uint32_t DLC, uint8_t *TxData)
{
	#ifdef USE_SN65HVD230
		Transmit_CAN_Message_SN65HVD230(hcan, StdId, DLC, TxData);
	#else
		Transmit_CAN_Message_MCP2515(StdId, DLC, TxData);
	#endif
}


/* MCP2515 Can Transceiver */

void Receive_CAN_Message_MCP2515()
{
    // Attempt to receive a CAN message
    if (CANSPI_Receive(&rxMessage)) {
        // Check for specific CAN message ID
    	switch(rxMessage.frame.id){
    		case 0x33:
    			// Extract data from CAN message
    			a.x = (uint8_t) rxMessage.frame.data0;
    			a.y = (uint8_t) rxMessage.frame.data1;
    			a.z = (uint8_t) rxMessage.frame.data2;

    			can_flags.msg0x33 = 1;
    			break;

    		case 0x034:
    			// Extract data from CAN message
    			g.x = (uint8_t) rxMessage.frame.data0;
				g.y = (uint8_t) rxMessage.frame.data1;
				g.z = (uint8_t) rxMessage.frame.data2;

				can_flags.msg0x34 = 1;
    			break;
    	}

        // Indicate successful reception by turning on the LED
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    } else {
        // Indicate reception error by turning off the LED
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    }
}

void Debug_Receive_CAN_Message_MCP2515()
{
    // Attempt to receive a CAN message
    if (CANSPI_Receive(&rxMessage)) {
        // Check for specific CAN message ID
    	switch(rxMessage.frame.id){
			case 0x35:
				// Extract data from CAN message
				s1 = (uint8_t) rxMessage.frame.data0;
				s2 = (uint8_t) rxMessage.frame.data1;
				s3 = (uint8_t) rxMessage.frame.data2;
				s4 = (uint8_t) rxMessage.frame.data3;

				can_flags.msg0x35 = 1;

				break;

			case 0x36:
				// Extract data from CAN message
				s5 = (uint8_t) rxMessage.frame.data0;

				can_flags.msg0x36 = 1;

				break;

			case 0x05:
				// Extract data from CAN message
				eChar = (char) rxMessage.frame.data0;
				sensorNumber = (uint8_t) rxMessage.frame.data1;

				can_flags.msg0x05 = 1;

				break;

			case 0x06:
				// Extract data from CAN message
				receivedChar = (char) rxMessage.frame.data0;

				can_flags.msg0x06 = 1;

				break;

			default:
				// Handle other CAN message IDs if needed
				break;
		}

        // Indicate successful reception by turning on the LED
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    } else {
        // Indicate reception error by turning off the LED
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    }
}

void Transmit_CAN_Message_MCP2515(uint32_t StdId, uint8_t DLC, uint8_t *TxData)
{
	// Configuring a CAN message with standard ID
	txMessage.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B; // Use dEXTENDED_CAN_MSG_ID_2_0B for 29-bit extended IDs

	// Standard 11-bit ID
	txMessage.frame.id = StdId;

	// Number of bytes sent (maximum 8 bytes)
	uint8_t maxLength = (DLC < 8) ? DLC : 8;
	txMessage.frame.dlc = maxLength;

	// Copy data from the TxData array to the fields of txMessage.frame.data:
	// txMessage.frame.data0; txMessage.frame.data1; txMessage.frame.data2; txMessage.frame.data3;
	// txMessage.frame.data4; txMessage.frame.data5; txMessage.frame.data6; txMessage.frame.data7;
	for (uint8_t i = 0; i < 8; i++) {
		// If index i is less than the actual length of the data, copy the data
		// Otherwise, assign 0 as the default value
		((uint8_t*)&txMessage.frame.data0)[i] = (i < maxLength) ? TxData[i] : 0;
	}

	// Try to add the message to the CAN bus
	if (CANSPI_Transmit(&txMessage)) {
		// Successfully queues the CAN message for transmission by finding an available buffer and requesting to send the data

		// Turn on the built-in LED to indicate successful transmission
		// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Green Led
	} else {
		// All transmission buffers are busy and thus no new message can be queued for transmission at that moment

		// Turn off the built-in LED to indicate an error during transmission
		// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		Error_Handler(); // Handle error if message transmission fails
	}
}


/* SN65HVD230 Can Transceiver */

void Receive_CAN_Message_SN65HVD230(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t RxData[8]; // Buffer to store received CAN data

	// Check if there is a CAN message available on CAN2
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
		// Turn on the built-in LED to indicate successful reception
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Green Led

		// Process CAN messages based on their IDs
		switch (RxHeader.StdId) {
		case 0x33:
			// Extract data from CAN message
			a.x = (uint8_t) RxData[0];
			a.y = (uint8_t) RxData[1];
			a.z = (uint8_t) RxData[2];

			can_flags.msg0x33 = 1;
			break;

		case 0x34:
			// Extract data from CAN message
			g.x = (uint8_t) RxData[0];
			g.y = (uint8_t) RxData[1];
			g.z = (uint8_t) RxData[2];

			can_flags.msg0x34 = 1;
			break;

		default:
			// Handle other CAN message IDs if needed
			break;
		}
	}else{
		// Turn off the built-in LED to indicate an error during reception
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	}
}

void Debug_Receive_CAN_Message_SN65HVD230(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t RxData[8]; // Buffer to store received CAN data

	// Check if there is a CAN message available on CAN2
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
		// Turn on the built-in LED to indicate successful reception
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Green Led

		// Process CAN messages based on their IDs
		switch (RxHeader.StdId) {
			case 0x35:
			// Extract data from CAN message
			s1 = (uint8_t) RxData[0];
			s2 = (uint8_t) RxData[1];
			s3 = (uint8_t) RxData[2];
			s4 = (uint8_t) RxData[3];

			can_flags.msg0x35 = 1;

			break;

		case 0x36:
			// Extract data from CAN message
			s5 = (uint8_t) RxData[0];

			can_flags.msg0x36 = 1;

			break;

		case 0x05:
			// Extract data from CAN message
			eChar = (char) RxData[0];
			sensorNumber = (uint8_t) RxData[1];

			can_flags.msg0x05 = 1;

			break;

		case 0x06:
			// Extract data from CAN message
			receivedChar = (char) RxData[0];

			can_flags.msg0x06 = 1;

			break;

		default:
			// Handle other CAN message IDs if needed
			break;
		}
	}else{
		// Turn off the built-in LED to indicate an error during reception
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	}
}

void Transmit_CAN_Message_SN65HVD230(CAN_HandleTypeDef *hcan, uint32_t StdId, uint32_t DLC, uint8_t *TxData)
{
	// Initialize CAN header
	CAN_TxHeaderTypeDef TxHeader;

	// Standard 11-bit ID
	TxHeader.StdId = StdId; // Use TxHeader.ExtId = StdId; for 29-bit extended IDs

	// Configuring a CAN message with standard ID
	TxHeader.IDE = CAN_ID_STD;

	// Request type: Data frame
	TxHeader.RTR = CAN_RTR_DATA;

	// Number of bytes sent (maximum 8 bytes)
	uint8_t maxLength = (DLC < 8) ? DLC : 8;
	TxHeader.DLC = maxLength;

	// Select Tx Mailbox
	uint32_t TxMailbox = CAN_TX_MAILBOX0;

	// Try to add the message to the CAN bus
	if (HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, &TxMailbox) == HAL_OK) {
		// Turn on the built-in LED to indicate successful transmission
		// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	} else {
		// Turn off the built-in LED to indicate an error during transmission
		// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		Error_Handler(); // Handle error if message transmission fails
	}
}

void Display_Can_Messages()
{
	if (can_flags.msg0x33){
		// Display received messages
		char msg_33[50];
		uint16_t msg_33_length;

		// Formatting and sending the message for the gyroscope
		msg_33_length = sprintf(msg_33, "Accelerometer: x = %d; y = %d; z = %d\n\n", a.x, a.y, a.z);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg_33, msg_33_length, HAL_MAX_DELAY);

		can_flags.msg0x33 = 0;
	}

	if (can_flags.msg0x34){
		// Display received messages
		char msg_34[50];
		uint16_t msg_34_length;

		// Formatting and sending the message for the gyroscope
		msg_34_length = sprintf(msg_34, "Gyroscope: x = %d; y = %d; z = %d\n\n", g.x, g.y, g.z);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg_34, msg_34_length, HAL_MAX_DELAY);

		can_flags.msg0x34 = 0;
	}

	if (can_flags.msg0x35){
		// Display received messages
		char msg_35[50];
		uint16_t msg_35_length;

		// Formatting and sending the message for the sensor 1
		msg_35_length = sprintf(msg_35, "Sensor 1: %d\n", s1);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg_35, msg_35_length, HAL_MAX_DELAY);

		// Formatting and sending the message for the sensor 2
		msg_35_length = sprintf(msg_35, "Sensor 2: %d\n", s2);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg_35, msg_35_length, HAL_MAX_DELAY);

		// Formatting and sending the message for the sensor 3
		msg_35_length = sprintf(msg_35, "Sensor 3: %d\n", s3);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg_35, msg_35_length, HAL_MAX_DELAY);

		// Formatting and sending the message for the sensor 4
		msg_35_length = sprintf(msg_35, "Sensor 4: %d\n", s4);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg_35, msg_35_length, HAL_MAX_DELAY);

		can_flags.msg0x35 = 0;
	}

	if (can_flags.msg0x36){
		// Display received messages
		char msg_36[50];
		uint16_t msg_36_length;

		// Formatting and sending the message for the sensor 5
		msg_36_length = sprintf(msg_36, "Sensor 5: %d\n\n", s5);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg_36, msg_36_length, HAL_MAX_DELAY);

		can_flags.msg0x36 = 0;
	}

	if (can_flags.msg0x05){
		// Display received messages
		char msg_05[50];
		uint16_t msg_05_length;

		// Formatting and sending the error message
		msg_05_length = sprintf(msg_05, "Error: %c%d\n\n", eChar, sensorNumber);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg_05, msg_05_length, HAL_MAX_DELAY);

		can_flags.msg0x05 = 0;
	}

	if (can_flags.msg0x06){
		// Display received messages
		char msg_06[50];
		uint16_t msg_06_length;

		msg_06_length = sprintf(msg_06, "Message: %c\n\n", receivedChar);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg_06, msg_06_length, HAL_MAX_DELAY);

		can_flags.msg0x06 = 0;
	}
}
