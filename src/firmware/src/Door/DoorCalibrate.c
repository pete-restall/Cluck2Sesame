#include <xc.h>
#include <stdint.h>

#include "../Platform/Event.h"
#include "../Platform/Motor.h"

#include "Door.h"

static void onDoorClosedForCalibration(const struct Event *event);
static void onDoorAbortedForCalibration(const struct Event *event);

static const struct EventSubscription onDoorClosedSubscription =
{
	.type = DOOR_CLOSED,
	.handler = &onDoorClosedForCalibration,
	.state = (void *) 0
};

static const struct EventSubscription onDoorAbortedSubscription =
{
	.type = DOOR_ABORTED,
	.handler = &onDoorAbortedForCalibration,
	.state = (void *) 0
};

static struct DoorCalibrated doorCalibrationEventArgs;

void doorCalibrate(void)
{
	motorEnable();
	doorStartFindingBottom(
		DoorState_FindBottom,
		DoorState_FindBottom_WaitingForEnabledMotor);

	eventSubscribe(&onDoorClosedSubscription);
	eventSubscribe(&onDoorAbortedSubscription);
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
		doorState.closed.loweredHeight = 0;
		doorState.findBottomIterations = 0;
	}
	else
		doorState.current = motorDisabledState;
}

static void onDoorClosedForCalibration(const struct Event *event)
{
	const struct DoorClosed *closed = (const struct DoorClosed *) event->args;
	// TODO: IF DOOR HEIGHT < (say) 100mm THEN IT'S A NONSENSE CONDITION - IT WASN'T RAISED HIGH ENOUGH.  SO SET A FAULT FLAG...
	doorCalibrationEventArgs.fault.all = 0;
	doorCalibrationEventArgs.height = (uint16_t) closed->loweredHeight;
	eventPublish(DOOR_CALIBRATED, &doorCalibrationEventArgs);
	eventUnsubscribe(&onDoorAbortedSubscription);
	eventUnsubscribe(&onDoorClosedSubscription);
}

static void onDoorAbortedForCalibration(const struct Event *event)
{
	const struct DoorAborted *aborted = (const struct DoorAborted *) event->args;
	doorCalibrationEventArgs.fault.all = aborted->fault.all;
	doorCalibrationEventArgs.height = 0;
	// TODO: SINCE NO DOOR_CALIBRATED EVENT IS PUBLISHED HERE, THE UI HANGS...
	eventUnsubscribe(&onDoorAbortedSubscription);
	eventUnsubscribe(&onDoorClosedSubscription);
}
