#include <xc.h>
#include <stdint.h>

#include "../Platform/NvmSettings.h"
#include "../ApplicationNvmSettings.h"
#include "../Motor.h"

#include "Door.h"

static void doorStartClosing(
	enum DoorState motorEnabledState,
	enum DoorState motorDisabledState);

void doorOnCloseScheduleActioned(const struct Event *event)
{
	switch (doorState.current)
	{
		case DoorState_Closed:
			doorState.transition = DoorTransition_Unchanged;
			break;

		case DoorState_Opened:
			doorStartClosing(
				DoorState_Closing,
				DoorState_Closing_WaitingForEnabledMotor);
			break;

		case DoorState_Unknown:
			doorStartClosing(
				DoorState_FindBottom,
				DoorState_FindBottom_WaitingForEnabledMotor);
			break;

		default:
			doorState.transition = DoorTransition_Close;
	};
}

static void doorStartClosing(
	enum DoorState motorEnabledState,
	enum DoorState motorDisabledState)
{
	motorEnable();
	if (motorIsEnabled())
	{
		motorLimitIsNoLoad();
		motorOn(-nvmSettings.application.door.height);
		doorState.current = motorEnabledState;
	}
	else
		doorState.current = motorDisabledState;

	doorState.transition = DoorTransition_Close;
}
