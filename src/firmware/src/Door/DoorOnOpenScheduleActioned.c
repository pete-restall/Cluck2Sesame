#include <xc.h>
#include <stdint.h>

#include "../Motor.h"

#include "Door.h"

void onDoorOpenScheduleActioned(const struct Event *event)
{
	switch (doorState.current)
	{
		case DoorState_Opened:
			doorState.transition = DoorTransition_Unchanged;
			break;

		case DoorState_Closed:
			motorEnable();
// TODO: IF ENABLED, SWITCH TO HIGH CURRENT MODE AND MOVE DOOR UP...
			doorState.current = motorIsEnabled()
				? DoorState_Opening
				: DoorState_Opening_WaitingForEnabledMotor;

			doorState.transition = DoorTransition_Open;
			break;

		case DoorState_Unknown:
			motorEnable();
// TODO: IF ENABLED, SWITCH TO LOW CURRENT MODE AND MOVE DOOR DOWN...
			doorState.current = DoorState_FindBottom;

		default:
			doorState.transition = DoorTransition_Open;
	};
}
