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

Vector a;
Vector g;


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

	// Configure the filter
	CAN_FilterTypeDef sFilterConfig;
	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig);

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
		case 0x033:
			// Extract data from CAN message with ID 0x033
			a.x = (uint8_t) RxData[0];
			a.y = (uint8_t) RxData[1];
			a.z = (uint8_t) RxData[2];
			break;

		case 0x034:
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

