#ifndef __CLUCK3SESAME_SRC_DOOR_H
#define __CLUCK3SESAME_SRC_DOOR_H
#include <stdint.h>
#include "Platform/Event.h"
#include "Platform/Clock.h"

union DoorFaults
{
	uint8_t all;
	struct
	{
		unsigned int isJammed : 1;
		unsigned int isLineSnapped : 1;
		unsigned int isEncoderBroken : 1;
		unsigned int isInsufficientPower : 1;
	};
};

#define DOOR_OPEN_SCHEDULE_ACTIONED ((EventType) 0x50)
struct DoorOpenScheduleActioned { EMPTY_EVENT_ARGS };

#define DOOR_CLOSE_SCHEDULE_ACTIONED ((EventType) 0x51)
struct DoorCloseScheduleActioned { EMPTY_EVENT_ARGS };

#define DOOR_OPENED ((EventType) 0x52)
struct DoorOpened { EMPTY_EVENT_ARGS };

#define DOOR_CLOSED ((EventType) 0x53)
struct DoorClosed { EMPTY_EVENT_ARGS };

#define DOOR_ABORTED ((EventType) 0x54)
struct DoorAborted
{
	union DoorFaults fault;
};

enum DoorTransition
{
	DoorTransition_Unchanged,
	DoorTransition_Open,
	DoorTransition_Close
};

enum DoorState
{
	DoorState_Unknown,
	DoorState_Fault,
	DoorState_FindBottom,
	DoorState_Opening,
	DoorState_Opening_WaitingForEnabledMotor,
	DoorState_Opened,
	DoorState_Closing,
	DoorState_Closed
};

struct DoorStateWithContext
{
	union
	{
		uint8_t all;
		struct
		{
			unsigned int isManuallyOverridden : 1;
			unsigned int isSunEventDriven : 1;
			unsigned int isTimeDriven : 1;
		};
	} flags;

	struct Time autoOpenTime;
	struct Time autoCloseTime;
	union DoorFaults fault;
	enum DoorTransition transition;
	enum DoorState current;
};

extern void doorInitialise(void);
extern void doorGetState(struct DoorStateWithContext *state);

#endif
