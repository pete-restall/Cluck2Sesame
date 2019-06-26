#include <xc.h>
#include <stdint.h>

#include "Door.h"

void onDoorOpenScheduleActioned(const struct Event *event)
{
	switch (doorState.actualState)
	{
		case DoorActualState_Opened:
		case DoorActualState_Fault:
			doorState.transition = DoorTransition_Unchanged;
			break;

		case DoorActualState_Unknown:
			doorState.actualState = DoorActualState_FindBottom;

		default:
			doorState.transition = DoorTransition_Open;
	};
}
