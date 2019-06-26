#include <xc.h>
#include <stdint.h>

#include "../Platform/FarScheduler.h"
#include "../Platform/NvmSettings.h"
#include "../ApplicationNvmSettings.h"
#include "../SunEvents.h"

#include "Door.h"

static void onDateOrNvmSettingsChanged(const struct Event *event);
static void onSunEventsChanged(const struct Event *event);

static const struct DoorOpenScheduleActioned openingScheduleEventArgs = { };
static struct FarSchedule openingSchedule =
{
	.eventType = DOOR_OPEN_SCHEDULE_ACTIONED,
	.eventArgs = &openingScheduleEventArgs
};

static const struct DoorCloseScheduleActioned closingScheduleEventArgs = { };
static struct FarSchedule closingSchedule =
{
	.eventType = DOOR_CLOSE_SCHEDULE_ACTIONED,
	.eventArgs = &closingScheduleEventArgs
};

struct DoorStateInternal doorState;

void doorInitialise(void)
{
	doorState.current = DoorState_Unknown;
	doorState.transition = DoorTransition_Unchanged;

	static const struct EventSubscription onDoorAbortedSubscription =
	{
		.type = DOOR_ABORTED,
		.handler = &onDoorAborted,
		.state = (void *) 0
	};

	eventSubscribe(&onDoorAbortedSubscription);

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
		.handler = &onDoorOpenScheduleActioned,
		.state = (void *) 0
	};

	eventSubscribe(&onDoorOpenScheduleActionedSubscription);
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
