#include <xc.h>
#include <stdint.h>

#include "../Platform/NvmSettings.h"
#include "../Platform/Motor.h"
#include "../ApplicationNvmSettings.h"

#include "Door.h"

void doorOnMotorEnabled(const struct Event *event)
{
	switch (doorState.current)
	{
		case DoorState_Opening_WaitingForEnabledMotor:
			doorStartOpening(DoorState_Opening, DoorState_Unknown);
			break;

		case DoorState_Closing_WaitingForEnabledMotor:
			doorStartClosing(DoorState_Closing, DoorState_Unknown);
			break;

		// TODO !
		//case DoorState_FindBottom_WaitingForEnabledMotor:

		default:
		break; // TODO: break TO BE REMOVED AND CURRENT STATE SET TO DoorState_Unknown
//			doorState.current = DoorState_Unknown;
	};
}
