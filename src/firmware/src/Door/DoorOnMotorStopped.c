#include <xc.h>
#include <stdint.h>

#include "../Platform/NvmSettings.h"
#include "../Platform/Motor.h"
#include "../ApplicationNvmSettings.h"

#include "Door.h"

void doorOnMotorStopped(const struct Event *event)
{
	const struct MotorStopped *args = (const struct MotorStopped *) event->args;
	uint8_t fault = 0;
	switch (doorState.current)
	{
		case DoorState_Opening:
			if (args->fault.any)
			{
				if (args->fault.currentLimited)
					fault = DOOR_JAMMED;

				goto motorFaulted;
			}

			if (doorState.transition != DoorTransition_Close)
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
				if (args->fault.currentLimited)
					fault = DOOR_REVERSED;

				goto motorFaulted;
			}

			if (doorState.transition != DoorTransition_Open)
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
			if (args->requestedCount < 0)
			{
				if (args->fault.any)
				{
					if (args->fault.currentLimited)
						fault = DOOR_REVERSED;

					goto motorFaulted;
				}

				motorOn(FIND_BOTTOM_RAISING);
				doorState.findBottomIterations++;
			}
			else
			{
				if (args->fault.currentLimited)
				{
					if (args->actualCount > FIND_BOTTOM_THRESHOLD)
					{
						eventPublish(DOOR_CLOSED, &doorState.closed);
						if (doorState.transition != DoorTransition_Open)
						{
							motorDisable();
							doorState.current = DoorState_Closed;
							doorState.transition = DoorTransition_Unchanged;
						}
						else
						{
							doorStartOpening(DoorState_Opening, DoorState_Opening);
						}
					}
					else
					{
						if (doorState.findBottomIterations == MAX_FIND_BOTTOM_ITERATIONS)
						{
							fault = LINE_TOO_LONG;
							goto motorFaulted;
						}

						motorOn(FIND_BOTTOM_LOWERING);
					}
				}
				else
				{
					if (args->fault.all == 0 || args->fault.encoderOverflow)
						fault = LINE_SNAPPED;

					goto motorFaulted;
				}
			}

			break;

		case DoorState_ManualOpening:
			if (args->fault.any)
			{
				if (args->fault.currentLimited)
					fault = DOOR_JAMMED;

				goto motorFaulted;
			}

			motorDisable();
			doorState.current = DoorState_Unknown;
			break;

		case DoorState_ManualClosing:
			if (args->fault.any)
			{
				if (args->fault.currentLimited)
					fault = DOOR_REVERSED;

				goto motorFaulted;
			}

			motorDisable();
			doorState.current = DoorState_Unknown;
			break;

		case DoorState_Fault:
		case DoorState_Unknown:
			break;

		case DoorState_Opening_WaitingForEnabledMotor:
		case DoorState_Closing_WaitingForEnabledMotor:
		case DoorState_ManualOpening_WaitingForEnabledMotor:
		case DoorState_ManualClosing_WaitingForEnabledMotor:
			motorDisable();

		default:
			doorState.current = DoorState_Unknown;
	};

	return;

motorFaulted:
	if (fault == 0)
	{
		if (args->fault.encoderTimeout)
			fault = ENCODER_BROKEN;

		if (args->fault.encoderOverflow)
			fault = LINE_TOO_LONG;
	}

	motorDisable();
	doorState.current = DoorState_Fault;
	doorState.aborted.fault.all = fault;
	eventPublish(DOOR_ABORTED, &doorState.aborted);
}
