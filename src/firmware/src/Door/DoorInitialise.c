#include <xc.h>

#include "../Platform/FarScheduler.h"
#include "../Platform/NvmSettings.h"
#include "../ApplicationNvmSettings.h"

#include "Door.h"

static void onDateChanged(const struct Event *const event);

void doorInitialise(void)
{
	static const struct EventSubscription onDateChangedSubscription =
	{
		.type = DATE_CHANGED,
		.handler = &onDateChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onDateChangedSubscription);
}

static void onDateChanged(const struct Event *const event)
{
	if (!nvmSettings.application.door.mode.isTimeDriven)
		return;

	static const struct DoorOpenScheduleActioned openingScheduleEventArgs = { };

	static struct FarSchedule openingSchedule =
	{
		.eventType = DOOR_OPEN_SCHEDULE_ACTIONED,
		.eventArgs = &openingScheduleEventArgs
	};

	openingSchedule.time.hour = nvmSettings.application.door.autoOpenTime.hour,
	openingSchedule.time.minute = nvmSettings.application.door.autoOpenTime.minute;

	farSchedulerAdd(&openingSchedule);

	static const struct DoorCloseScheduleActioned closingScheduleEventArgs = { };

	static struct FarSchedule closingSchedule =
	{
		.eventType = DOOR_CLOSE_SCHEDULE_ACTIONED,
		.eventArgs = &closingScheduleEventArgs
	};

	closingSchedule.time.hour = nvmSettings.application.door.autoCloseTime.hour,
	closingSchedule.time.minute = nvmSettings.application.door.autoCloseTime.minute;

	farSchedulerAdd(&closingSchedule);
}
