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
			doorStartFindingBottom();

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
		// TODO: SUSPECT THAT COASTING THE MOTOR WILL HAVE OVERRUN SLIGHTLY ON THE DOWNWARD LEG.  BETTER TO RAISE BY THE LAST LOWERING-COUNT.  SAME GOES FOR THE REVERSAL CODE, WHEN JUST CLOSED.
		motorOn(nvmSettings.application.door.height);
		doorState.current = motorEnabledState;
	}
	else
		doorState.current = motorDisabledState;
}

void doorStartFindingBottom(void)
{
	if (motorIsEnabled())
	{
		motorLimitIsNoLoad();
		motorOn(FIND_BOTTOM_LOWERING);
		doorState.current = DoorState_FindBottom;
	}
	else
		doorState.current = DoorState_FindBottom_WaitingForEnabledMotor;

	doorState.findBottomIterations = 0;
}
