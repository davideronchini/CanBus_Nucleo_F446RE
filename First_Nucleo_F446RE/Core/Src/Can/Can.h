#ifndef __CAN_H
#define __CAN_H

/* TODO Headers needed to this module to be included. */
#include "main.h"

/* TODO Defines, variables and functions to be visible (exported) to other modules that includes "Scheduler.h". */
typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t z;
} Vector;

extern Vector a;
extern Vector g;

extern void Transmit_CAN_Message(CAN_HandleTypeDef *, uint32_t, uint32_t, uint8_t *);
extern void Receive_CAN_Message(CAN_HandleTypeDef *);

#endif /* __CAN_H */

