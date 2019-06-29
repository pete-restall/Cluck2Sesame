#ifndef __CLUCK3SESAME_SRC_DOOR_DOOR_H
#define __CLUCK3SESAME_SRC_DOOR_DOOR_H
#include "../Event.h"
#include "../Door.h"

struct DoorStateInternal
{
	enum DoorState current;
	enum DoorTransition transition;
	struct DoorAborted aborted;
};

extern struct DoorStateInternal doorState;

extern void doorOnOpenScheduleActioned(const struct Event *event);
extern void doorOnCloseScheduleActioned(const struct Event *event);
extern void doorOnMotorStopped(const struct Event *event);

#endif
