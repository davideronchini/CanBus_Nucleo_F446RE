#ifndef __CAN_H
#define __CAN_H

/* TODO Headers needed to this module to be included. */
#include "Scheduler.h"
#include "CAN_SPI.h"
#include "main.h"

/* TODO Defines, variables and functions to be visible (exported) to other modules that includes "Scheduler.h". */
typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t z;
} Vector;

extern Vector a;
extern Vector g;

extern void CanInit(void);

extern void Receive_CAN_Message();
extern void Transmit_CAN_Message(uint32_t StdId, uint8_t DLC, uint8_t *TxData);

//extern void Transmit_CAN_Message(CAN_HandleTypeDef *, uint32_t, uint32_t, uint8_t *);

#endif /* __CAN_H */

