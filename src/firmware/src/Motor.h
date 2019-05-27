#ifndef __CLUCK3SESAME_SRC_MOTOR_H
#define __CLUCK3SESAME_SRC_MOTOR_H
#include <stdint.h>
#include "Event.h"

#define MOTOR_ENABLED ((EventType) 0x38)
struct MotorEnabled { EMPTY_EVENT_ARGS };

#define MOTOR_DISABLED ((EventType) 0x39)
struct MotorDisabled { EMPTY_EVENT_ARGS };

#define MOTOR_STARTED ((EventType) 0x3a)
struct MotorStarted
{
	int16_t count;
};

#define MOTOR_STOPPED ((EventType) 0x3b)
struct MotorStopped
{
	int16_t actualCount;
	int16_t requestedCount;
	union
	{
		uint8_t all;
		struct
		{
			unsigned int currentLimited : 1;
			unsigned int encoderTimeout : 1;
			unsigned int encoderOverflow : 1;
		};
	} fault;
};

extern void motorInitialise(void);
extern void motorEnable(void);
extern uint8_t motorIsEnabled(void);
extern void motorDisable(void);
extern void motorOn(int16_t count);
extern void motorOff(void);

#endif
