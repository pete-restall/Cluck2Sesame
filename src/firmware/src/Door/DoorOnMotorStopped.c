#include <xc.h>
#include <stdint.h>

#include "../Motor.h"

#include "Door.h"

void doorOnMotorStopped(const struct Event *event)
{
	const struct MotorStopped *args = (const struct MotorStopped *) event->args;
	switch (doorState.current)
	{
		case DoorState_Opening:
			if (args->fault.any)
			{
				motorDisable();
				doorState.current = DoorState_Fault;
				// TODO: PUBLISH DOOR_ABORTED
			}
			else if (doorState.transition != DoorTransition_Close)
			{
				motorDisable();
				doorState.current = DoorState_Opened;
				doorState.transition = DoorTransition_Unchanged;
				// TODO: PUBLISH DOOR_OPENED
			}
			else
			{
				// TODO: PUBLISH DOOR_OPENED
				// TODO: SET CURRENT LIMIT TO NO-LOAD
				// TODO: TURN THE MOTOR ON TO CLOSE THE DOOR
				// TODO: SET THE STATE TO DoorState_Closing
			}

			break;

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
