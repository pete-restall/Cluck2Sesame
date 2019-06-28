#include <xc.h>
#include <stdint.h>

#include "../Motor.h"

#include "Door.h"

void doorOnMotorStopped(const struct Event *event)
{
	switch (doorState.current)
	{
		case DoorState_Fault:
		case DoorState_Unknown:
			break;

		case DoorState_Opening_WaitingForEnabledMotor:
		case DoorState_Closing_WaitingForEnabledMotor:
			motorDisable();

		default:
			doorState.current = DoorState_Unknown;
	};
}
