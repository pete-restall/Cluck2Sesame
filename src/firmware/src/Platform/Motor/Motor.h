#ifndef __CLUCK2SESAME_SRC_PLATFORM_MOTOR_MOTOR_H
#define __CLUCK2SESAME_SRC_PLATFORM_MOTOR_MOTOR_H
#include "../Motor.h"

#define CWG1STR_STEERING_MASK ( \
	_CWG1STR_STRA_MASK | \
	_CWG1STR_STRB_MASK | \
	_CWG1STR_STRC_MASK | \
	_CWG1STR_STRD_MASK)

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
