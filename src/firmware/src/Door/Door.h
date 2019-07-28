#ifndef __CLUCK3SESAME_SRC_DOOR_DOOR_H
#define __CLUCK3SESAME_SRC_DOOR_DOOR_H
#include "../Platform/Event.h"
#include "../Door.h"

#define FIND_BOTTOM_LOWERING -1576
#define FIND_BOTTOM_RAISING 15756
#define FIND_BOTTOM_THRESHOLD 3
#define MAX_FIND_BOTTOM_ITERATIONS 10

#define DOOR_JAMMED 1
#define DOOR_REVERSED 2
#define LINE_SNAPPED 4
#define LINE_TOO_LONG 8
#define ENCODER_BROKEN 16

struct DoorStateInternal
{
	enum DoorState current;
	enum DoorTransition transition;
	struct DoorOpened opened;
	struct DoorClosed closed;
	struct DoorAborted aborted;
	uint8_t findBottomIterations;
};

extern struct DoorStateInternal doorState;

extern void doorOnOpenScheduleActioned(const struct Event *event);
extern void doorStartOpening(
	enum DoorState motorEnabledState,
	enum DoorState motorDisabledState);

extern void doorStartFindingBottom(void);

extern void doorOnCloseScheduleActioned(const struct Event *event);
extern void doorStartClosing(
	enum DoorState motorEnabledState,
	enum DoorState motorDisabledState);

extern void doorOnMotorStopped(const struct Event *event);

#endif
