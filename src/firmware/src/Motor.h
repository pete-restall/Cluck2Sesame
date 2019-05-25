#ifndef __CLUCK3SESAME_SRC_MOTOR_H
#define __CLUCK3SESAME_SRC_MOTOR_H
#include <stdint.h>
#include "Event.h"

#define MOTOR_STARTED ((EventType) 0x38)
struct MotorStarted
{
	int16_t count;
};

#define MOTOR_STOPPED ((EventType) 0x39)
struct MotorStopped
{
	int16_t actualCount;
	int16_t requestedCount;
	union
	{
		uint8_t all;
		struct
		{
			unsigned int manuallyStopped : 1;
			unsigned int currentLimited : 1;
		};
	} flags;
};

extern void motorInitialise(void);
extern void motorEnable(void);
extern void motorDisable(void);
extern void motorOn(int16_t count);
extern void motorOff(void);

#endif