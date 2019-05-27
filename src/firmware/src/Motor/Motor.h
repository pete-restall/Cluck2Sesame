#ifndef __CLUCK3SESAME_SRC_MOTOR_MOTOR_H
#define __CLUCK3SESAME_SRC_MOTOR_MOTOR_H
#include "../Motor.h"

struct MotorState
{
	uint8_t enableCount;
	union
	{
		uint8_t all;
		struct
		{
			unsigned int isFullyEnabled : 1;
		};
	} flags;
};

extern struct MotorState motorState;

#endif
