#ifndef __CAN_H
#define __CAN_H

/* TODO Headers needed to this module to be included. */
#include "Scheduler.h"
#include "main.h"

/* TODO Private types */
typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t z;
} Vector;

extern Vector a;
extern Vector g;

/* TODO Defines, variables and functions to be visible (exported) to other modules that includes "Scheduler.h". */
extern void CanInit(void);

extern void Receive_CAN_Message(CAN_HandleTypeDef *);

#endif /* __CAN_H */

