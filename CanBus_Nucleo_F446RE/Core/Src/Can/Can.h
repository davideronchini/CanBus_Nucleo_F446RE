#ifndef __CAN_H
#define __CAN_H

/* TODO Headers needed to this module to be included. */
#include "Scheduler.h"
#include "CAN_SPI.h"
#include "main.h"

/* TODO Defines, variables and functions to be visible (exported) to other modules that includes "Scheduler.h". */

/*
 * Define this macro if you use the SN65HVD230 can bus transceiver,
 * otherwise commenting it will use the code of the MCP2515 can bus transceiver.
 *
 */
#define USE_SN65HVD230

typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t z;
} Vector;

extern Vector a; // Accelerometer
extern Vector g; // Gyroscope

extern void CanInit(void);

extern void Transmit_CAN_Message(CAN_HandleTypeDef *, uint32_t, uint32_t, uint8_t *);

extern void Receive_CAN_Message_MCP2515();
extern void Transmit_CAN_Message_MCP2515(uint32_t StdId, uint8_t DLC, uint8_t *TxData);

extern void Receive_CAN_Message_SN65HVD230(CAN_HandleTypeDef *);
extern void Transmit_CAN_Message_SN65HVD230(CAN_HandleTypeDef *, uint32_t, uint32_t, uint8_t *);

// Debug logic to verify the correct functioning of the main board.
// This logic must be loaded into the debugging board by enabling DEBUG_MODE
extern void Debug_Receive_CAN_Message_MCP2515();
extern void Debug_Receive_CAN_Message_SN65HVD230(CAN_HandleTypeDef *);

extern void Display_Can_Messages(void);

#endif /* __CAN_H */

