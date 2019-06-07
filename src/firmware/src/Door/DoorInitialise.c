#include <xc.h>

#include "../Platform/FarScheduler.h"
#include "../Platform/NvmSettings.h"
#include "../ApplicationNvmSettings.h"

#include "Door.h"

void doorInitialise(void)
{
	static const struct DoorOpenScheduleActioned openingScheduleEventArgs = { };

	static struct FarSchedule openingSchedule =
	{
		.eventType = DOOR_OPEN_SCHEDULE_ACTIONED,
		.eventArgs = &openingScheduleEventArgs
	};

	openingSchedule.time.hour = nvmSettings.application.door.autoOpenTime.hour,
	openingSchedule.time.minute = nvmSettings.application.door.autoOpenTime.minute;

	farSchedulerAdd(&openingSchedule);
}
