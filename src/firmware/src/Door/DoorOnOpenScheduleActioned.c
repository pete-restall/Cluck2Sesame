#include <xc.h>
#include <stdint.h>

#include "Door.h"

void onDoorOpenScheduleActioned(const struct Event *event)
{
	switch (doorState.current)
	{
		case DoorState_Opened:
		case DoorState_Fault:
			doorState.transition = DoorTransition_Unchanged;
			break;

		case DoorState_Unknown:
			doorState.current = DoorState_FindBottom;

		default:
			doorState.transition = DoorTransition_Open;
	};
}
