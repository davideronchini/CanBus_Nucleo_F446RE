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

Vector a = {0};
Vector g = {0};

GPIO_PinState led_state = GPIO_PIN_SET;


/**
 *
 *  Functions
 *
 *
 */
void CanInit(void)
{
	/* Executed once at startup. */
	HAL_CAN_Start(&hcan2);

	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void Receive_CAN_Message(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t RxData[8]; // Buffer to store received CAN data

	// Check if there is a CAN message available on CAN2
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
		// Turn on the built-in LED to indicate successful reception
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Green Led

		// Process CAN messages based on their IDs
		switch (RxHeader.StdId) {
		case 0x33:
			// Extract data from CAN message with ID 0x033
			a.x = (uint8_t) RxData[0];
			a.y = (uint8_t) RxData[1];
			a.z = (uint8_t) RxData[2];
			break;

		case 0x34:
			// Extract data from CAN message with ID 0x034
			g.x = (uint8_t) RxData[0];
			g.y = (uint8_t) RxData[1];
			g.z = (uint8_t) RxData[2];
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

