#ifndef __CLUCK3SESAME_SRC_DOOR_H
#define __CLUCK3SESAME_SRC_DOOR_H
#include <stdint.h>
#include "Platform/Event.h"
#include "Platform/Clock.h"

union DoorFaults
{
	uint8_t all;
	uint8_t any;
	struct
	{
		unsigned int isJammed : 1;
		unsigned int isReversed : 1;
		unsigned int isLineSnapped : 1;
		unsigned int isLineTooLong : 1;
		unsigned int isEncoderBroken : 1;
		unsigned int isInsufficientPower : 1;
	};
};

#define DOOR_OPEN_SCHEDULE_ACTIONED ((EventType) 0x58)
struct DoorOpenScheduleActioned { EMPTY_EVENT_ARGS };

#define DOOR_CLOSE_SCHEDULE_ACTIONED ((EventType) 0x59)
struct DoorCloseScheduleActioned { EMPTY_EVENT_ARGS };

#define DOOR_OPENED ((EventType) 0x5a)
struct DoorOpened { EMPTY_EVENT_ARGS };

#define DOOR_CLOSED ((EventType) 0x5b)
struct DoorClosed { EMPTY_EVENT_ARGS };

#define DOOR_ABORTED ((EventType) 0x5c)
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
	DoorState_ManualOpening_WaitingForEnabledMotor,
	DoorState_ManualOpening,
	DoorState_ManualClosing_WaitingForEnabledMotor,
	DoorState_ManualClosing,
	DoorState_FindBottom,
	DoorState_FindBottom_WaitingForEnabledMotor,
	DoorState_Opening,
	DoorState_Opening_WaitingForEnabledMotor,
	DoorState_Opened,
	DoorState_Closing,
	DoorState_Closing_WaitingForEnabledMotor,
	DoorState_Closed
};

struct DoorStateWithContext
{
	union
	{
		uint8_t all;
		uint8_t any;
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
extern void doorManualStartOpening(void);
extern void doorManualStartClosing(void);
extern void doorManualStop(void);

#endif
