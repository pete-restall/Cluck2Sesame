#include <xc.h>
#include <stdint.h>

#include "../Platform/Event.h"
#include "../Platform/FarScheduler.h"
#include "../Platform/NvmSettings.h"
#include "../Platform/Motor.h"
#include "../ApplicationNvmSettings.h"
#include "../SunEvents.h"

#include "Door.h"

static void onDateOrNvmSettingsChanged(const struct Event *event);
static void onSunEventsChanged(const struct Event *event);

static struct FarSchedule openingSchedule =
{
	.eventType = DOOR_OPEN_SCHEDULE_ACTIONED
};

static struct FarSchedule closingSchedule =
{
	.eventType = DOOR_CLOSE_SCHEDULE_ACTIONED
};

struct DoorStateInternal doorState;

void doorInitialise(void)
{
	doorState.current = DoorState_Unknown;
	doorState.transition = DoorTransition_Unchanged;

	static const struct EventSubscription onDateChangedSubscription =
	{
		.type = DATE_CHANGED,
		.handler = &onDateOrNvmSettingsChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onDateChangedSubscription);

	static const struct EventSubscription onNvmSettingsChangedSubscription =
	{
		.type = NVM_SETTINGS_CHANGED,
		.handler = &onDateOrNvmSettingsChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onNvmSettingsChangedSubscription);

	static const struct EventSubscription onSunEventsChangedSubscription =
	{
		.type = SUN_EVENTS_CHANGED,
		.handler = &onSunEventsChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onSunEventsChangedSubscription);

	static const struct EventSubscription onDoorOpenScheduleActionedSubscription =
	{
		.type = DOOR_OPEN_SCHEDULE_ACTIONED,
		.handler = &doorOnOpenScheduleActioned,
		.state = (void *) 0
	};

	eventSubscribe(&onDoorOpenScheduleActionedSubscription);

	static const struct EventSubscription onDoorCloseScheduleActionedSubscription =
	{
		.type = DOOR_CLOSE_SCHEDULE_ACTIONED,
		.handler = &doorOnCloseScheduleActioned,
		.state = (void *) 0
	};

	eventSubscribe(&onDoorCloseScheduleActionedSubscription);

	static const struct EventSubscription onMotorStoppedSubscription =
	{
		.type = MOTOR_STOPPED,
		.handler = &doorOnMotorStopped,
		.state = (void *) 0
	};

	eventSubscribe(&onMotorStoppedSubscription);

	static const struct EventSubscription onMotorEnabledSubscription =
	{
		.type = MOTOR_ENABLED,
		.handler = &doorOnMotorEnabled,
		.state = (void *) 0
	};

	eventSubscribe(&onMotorEnabledSubscription);

	// TODO: MotorEnabled - changes state to Opening, Closing, FindBottom if {Opening,Closing,FindBottom}_WaitingForEnabledMotor, and starts the motor turning; DOES *NOT* MODIFY THE TRANSITION, HOWEVER...
}

static void onDateOrNvmSettingsChanged(const struct Event *event)
{
	farSchedulerRemove(&openingSchedule);
	farSchedulerRemove(&closingSchedule);

	if (!nvmSettings.application.door.mode.isTimeDriven)
		return;

	openingSchedule.time.hour = nvmSettings.application.door.autoOpenTime.hour,
	openingSchedule.time.minute = nvmSettings.application.door.autoOpenTime.minute;
	farSchedulerAdd(&openingSchedule);

	closingSchedule.time.hour = nvmSettings.application.door.autoCloseTime.hour,
	closingSchedule.time.minute = nvmSettings.application.door.autoCloseTime.minute;
	farSchedulerAdd(&closingSchedule);
}

static void onSunEventsChanged(const struct Event *event)
{
	if (!nvmSettings.application.door.mode.isSunEventDriven)
		return;

	const struct SunEventsChanged *args = (const struct SunEventsChanged *) event->args;

	// TODO: OFFSETS FROM nvmSettings.application.door.sunEvents NEED ADDING !
	farSchedulerRemove(&openingSchedule);
	openingSchedule.time.hour = args->sunrise.hour,
	openingSchedule.time.minute = args->sunrise.minute;
	farSchedulerAdd(&openingSchedule);

	farSchedulerRemove(&closingSchedule);
	closingSchedule.time.hour = args->sunset.hour,
	closingSchedule.time.minute = args->sunset.minute;
	farSchedulerAdd(&closingSchedule);
}
