#ifndef __CLUCK3SESAME_SRC_DOOR_DOOR_H
#define __CLUCK3SESAME_SRC_DOOR_DOOR_H
#include "../Event.h"
#include "../Door.h"

struct DoorStateInternal
{
	enum DoorState current;
	enum DoorTransition transition;
};

extern struct DoorStateInternal doorState;

extern void onDoorAborted(const struct Event *event);
extern void onDoorOpenScheduleActioned(const struct Event *event);
extern void onDoorCloseScheduleActioned(const struct Event *event);

#endif
