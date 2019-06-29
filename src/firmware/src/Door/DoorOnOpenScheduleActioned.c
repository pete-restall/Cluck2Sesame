#include <xc.h>
#include <stdint.h>

#include "../Platform/NvmSettings.h"
#include "../ApplicationNvmSettings.h"
#include "../Motor.h"

#include "Door.h"

void doorOnOpenScheduleActioned(const struct Event *event)
{
	switch (doorState.current)
	{
		case DoorState_Opened:
			doorState.transition = DoorTransition_Unchanged;
			break;

		case DoorState_Closed:
			motorEnable();
			doorStartOpening(
				DoorState_Opening,
				DoorState_Opening_WaitingForEnabledMotor);
			break;

		case DoorState_Unknown:
			motorEnable();
			doorStartOpening(
				DoorState_FindBottom,
				DoorState_FindBottom_WaitingForEnabledMotor);
			break;

		default:
			doorState.transition = DoorTransition_Open;
	};
}

void doorStartOpening(
	enum DoorState motorEnabledState,
	enum DoorState motorDisabledState)
{
	if (motorIsEnabled())
	{
		motorLimitIsMaximumLoad();
		// TODO: SUSPECT THAT COASTING THE MOTOR WILL HAVE OVERRUN SLIGHTLY ON THE DOWNWARD LEG.  BETTER TO RAISE BY THE LAST LOWERING-COUNT.
		motorOn(nvmSettings.application.door.height);
		doorState.current = motorEnabledState;
	}
	else
		doorState.current = motorDisabledState;

	doorState.transition = DoorTransition_Open;
}
