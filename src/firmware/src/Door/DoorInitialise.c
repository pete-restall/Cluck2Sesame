#include <xc.h>

#include "../Platform/FarScheduler.h"
#include "../Platform/NvmSettings.h"
#include "../ApplicationNvmSettings.h"

#include "Door.h"

static void onDateOrNvmSettingsChanged(const struct Event *const event);

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

void doorInitialise(void)
{
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
}

static void onDateOrNvmSettingsChanged(const struct Event *const event)
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

// TODO: VERIFY THAT SUN EVENT SCHEDULE USES SAME POINTERS AS TIME SCHEDULES, SINCE OTHERWISE WE MAY END UP WITH TWO SETS OF SCHEDULES BEING FIRED OFF
