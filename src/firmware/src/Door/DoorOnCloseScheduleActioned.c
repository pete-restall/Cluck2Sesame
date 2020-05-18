#include <xc.h>
#include <stdint.h>

#include "../Platform/NvmSettings.h"
#include "../Platform/Motor.h"
#include "../ApplicationNvmSettings.h"

#include "Door.h"

void doorOnCloseScheduleActioned(const struct Event *event)
{
	switch (doorState.current)
	{
		case DoorState_Closed:
			doorState.transition = DoorTransition_Unchanged;
			break;

		case DoorState_Opened:
			motorEnable();
			doorStartClosing(
				DoorState_Closing,
				DoorState_Closing_WaitingForEnabledMotor);

			doorState.transition = DoorTransition_Close;
			break;

		case DoorState_Unknown:
			motorEnable();
			doorStartFindingBottom(
				DoorState_FindBottom,
				DoorState_FindBottom_WaitingForEnabledMotor);

		default:
			doorState.transition = DoorTransition_Close;
	};
}

void doorStartClosing(
	enum DoorState motorEnabledState,
	enum DoorState motorDisabledState)
{
	if (motorIsEnabled())
	{
		motorLimitIsNoLoad();
		motorOn((motorEnabledState == DoorState_ManualClosing)
			? -MANUAL_MOVEMENT_LIMIT
			: -nvmSettings.application.door.height);

		doorState.current = motorEnabledState;
	}
	else
		doorState.current = motorDisabledState;
}
