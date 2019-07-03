#include <xc.h>
#include <stdint.h>

#include "../Platform/NvmSettings.h"
#include "../ApplicationNvmSettings.h"
#include "../Motor.h"

#include "Door.h"

#define DOOR_JAMMED 1
#define DOOR_REVERSED 2
#define LINE_SNAPPED 4
#define LINE_TOO_LONG 8
#define ENCODER_BROKEN 16

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
				doorState.aborted.fault.all =
					args->fault.currentLimited
						? DOOR_JAMMED
						: args->fault.encoderTimeout
							? ENCODER_BROKEN
							: args->fault.encoderOverflow
								? LINE_TOO_LONG
								: 0;

				eventPublish(DOOR_ABORTED, &doorState.aborted);
			}
			else if (doorState.transition != DoorTransition_Close)
			{
				motorDisable();
				doorState.current = DoorState_Opened;
				doorState.transition = DoorTransition_Unchanged;
				eventPublish(DOOR_OPENED, &doorState.opened);
			}
			else
			{
				eventPublish(DOOR_OPENED, &doorState.opened);
				doorStartClosing(DoorState_Closing, DoorState_Closing);
			}

			break;

		case DoorState_Closing:
			if (args->fault.any)
			{
				motorDisable();
				doorState.current = DoorState_Fault;
				doorState.aborted.fault.all =
					args->fault.currentLimited
						? DOOR_REVERSED
						: args->fault.encoderTimeout
							? ENCODER_BROKEN
							: args->fault.encoderOverflow
								? LINE_TOO_LONG
								: 0;

				eventPublish(DOOR_ABORTED, &doorState.aborted);
			}
			else if (doorState.transition != DoorTransition_Open)
			{
				motorDisable();
				doorState.current = DoorState_Closed;
				doorState.transition = DoorTransition_Unchanged;
				eventPublish(DOOR_CLOSED, &doorState.closed);
			}
			else
			{
				eventPublish(DOOR_CLOSED, &doorState.closed);
				doorStartOpening(DoorState_Opening, DoorState_Opening);
			}

			break;

		case DoorState_FindBottom:
			if (args->fault.any)
			{
				motorDisable();
				doorState.current = DoorState_Fault;
				doorState.aborted.fault.all =
					args->fault.currentLimited
						? DOOR_REVERSED
						: args->fault.encoderTimeout
							? ENCODER_BROKEN
							: args->fault.encoderOverflow
								? LINE_TOO_LONG
								: 0;

				eventPublish(DOOR_ABORTED, &doorState.aborted);
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
