#include <xc.h>
#include <stdint.h>

#include "../Platform/NvmSettings.h"
#include "../Platform/Motor.h"
#include "../ApplicationNvmSettings.h"

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

			doorState.transition = DoorTransition_Open;
			break;

		case DoorState_Unknown:
			motorEnable();
			doorStartFindingBottom(
				DoorState_FindBottom,
				DoorState_FindBottom_WaitingForEnabledMotor);

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
		motorOn(nvmSettings.application.door.height);
		doorState.current = motorEnabledState;
	}
	else
		doorState.current = motorDisabledState;
}

void doorStartFindingBottom(
	enum DoorState motorEnabledState,
	enum DoorState motorDisabledState)
{
	if (motorIsEnabled())
	{
		motorLimitIsNoLoad();
		motorOn(FIND_BOTTOM_LOWERING);
		doorState.current = motorEnabledState;
	}
	else
		doorState.current = motorDisabledState;

	doorState.findBottomIterations = 0;
}
