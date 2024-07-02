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


/**
 *
 *  Functions
 *
 *
 */
void CanInit(void)
{
	/* Executed once at startup. */
	HAL_CAN_Start(&hcan1);
}

void Transmit_CAN_Message(CAN_HandleTypeDef *hcan, uint32_t StdId, uint32_t DLC, uint8_t *TxData) {
	CAN_TxHeaderTypeDef TxHeader;

	// Initialize CAN header
	TxHeader.StdId = StdId;      // Standard Identifier (0 to 0x7FF)
	TxHeader.IDE = CAN_ID_STD;   // Use standard identifiers
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = DLC;    // Data Length Code: Number of bytes to send (0 to 8)

	// Select Tx Mailbox
	uint32_t TxMailbox = CAN_TX_MAILBOX0;

	// Try to add the message to the CAN bus
	if (HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, &TxMailbox) == HAL_OK) {
		// Turn on the built-in LED to indicate successful transmission
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);; // Green Led
	} else {
		// Turn off the built-in LED to indicate an error during transmission
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		Error_Handler(); // Handle error if message transmission fails
	}
}

