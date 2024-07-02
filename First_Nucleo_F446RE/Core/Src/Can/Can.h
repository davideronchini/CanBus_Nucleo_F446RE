#ifndef __CAN_H
#define __CAN_H

/* TODO Headers needed to this module to be included. */
#include "Scheduler.h"
#include "main.h"

/* TODO Defines, variables and functions to be visible (exported) to other modules that includes "Scheduler.h". */
extern void CanInit(void);

extern void Transmit_CAN_Message(CAN_HandleTypeDef *, uint32_t, uint32_t, uint8_t *);

#endif /* __CAN_H */

