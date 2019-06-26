#include <xc.h>
#include <stdint.h>

#include "Door.h"

void onDoorOpenScheduleActioned(const struct Event *event)
{
	if (
		doorState.actualState == DoorActualState_Opened ||
		doorState.actualState == DoorActualState_Fault)
		doorState.transition = DoorTransition_Unchanged;
	else
		doorState.transition = DoorTransition_Open;
}
