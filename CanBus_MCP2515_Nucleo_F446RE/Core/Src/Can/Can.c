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


/**
 *
 *  Functions
 *
 *
 */
void CanInit(void)
{
	/* Executed once at startup. */

	int result = CANSPI_Initialize();
	if(result < 0){
		// TODO: Handle the error
	}
}

void Receive_CAN_Message() {
    // Attempt to receive a CAN message
    if (CANSPI_Receive(&rxMessage)) {
        // Check for specific CAN message ID
    	switch(rxMessage.frame.id){
    		case 0x33:
    			// Extract data from CAN message
    			a.x = (uint8_t) rxMessage.frame.data0;
    			a.y = (uint8_t) rxMessage.frame.data1;
    			a.z = (uint8_t) rxMessage.frame.data2;
    			break;

    		case 0x034:
    			// Extract data from CAN message
    			g.x = (uint8_t) rxMessage.frame.data0;
				g.y = (uint8_t) rxMessage.frame.data1;
				g.z = (uint8_t) rxMessage.frame.data2;
    			break;
    	}

        // Indicate successful reception by turning on the LED
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    } else {
        // Indicate reception error by turning off the LED
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    }
}

void Transmit_CAN_Message(uint32_t StdId, uint8_t DLC, uint8_t *TxData){
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
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Green Led
	} else {
		// All transmission buffers are busy and thus no new message can be queued for transmission at that moment

		// Turn off the built-in LED to indicate an error during transmission
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		Error_Handler(); // Handle error if message transmission fails
	}
}
