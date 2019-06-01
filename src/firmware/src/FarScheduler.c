#include <xc.h>
#include <stdint.h>

#include "Event.h"
#include "Clock.h"
#include "FarScheduler.h"

#define MAX_SCHEDULES 32

struct FarScheduleWithFlags
{
	uint8_t dateDiscriminator;
	const struct FarSchedule *data;
};

static void buggyCompilerWorkaround(void)
{
	static const struct FarSchedule dummy = { .eventArgs = _OMNITARGET };
	static const struct FarScheduleWithFlags dummy2 = { .data = _OMNITARGET };
}

static void onDateChanged(const struct Event *const event);
static void onTimeChanged(const struct Event *const event);

static uint8_t dateDiscriminator;
static struct FarScheduleWithFlags schedules[MAX_SCHEDULES];
static struct FarScheduleWithFlags *const noMoreSchedules =
	schedules + MAX_SCHEDULES;

void farSchedulerInitialise(void)
{
	static const struct EventSubscription onDateChangedSubscription =
	{
		.type = DATE_CHANGED,
		.handler = &onDateChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onDateChangedSubscription);

	static const struct EventSubscription onTimeChangedSubscription =
	{
		.type = TIME_CHANGED,
		.handler = &onTimeChanged,
		.state = (void *) 0
	};

	eventSubscribe(&onTimeChangedSubscription);

	for (uint8_t i = 0; i < MAX_SCHEDULES; i++)
		schedules[i].data = (const struct FarSchedule *) 0;
}

static void onDateChanged(const struct Event *const event)
{
	dateDiscriminator++;
}

static void onTimeChanged(const struct Event *const event)
{
	// TODO: OBVIOUSLY MORE THAN JUST schedules[0]...
	const struct TimeChanged *args = (const struct TimeChanged *) event->args;
	if (schedules[0].dateDiscriminator == dateDiscriminator)
	{
		const struct FarSchedule *schedule = schedules[0].data;
		if (
			schedule->time.hour == args->now->hour &&
			schedule->time.minute == args->now->minute)
			eventPublish(schedule->eventType, schedule->eventArgs);
	}
}

void farSchedulerAdd(const struct FarSchedule *const schedule)
{
	// TODO: OBVIOUSLY MORE THAN JUST schedules[0]...
	schedules[0].dateDiscriminator = dateDiscriminator;
	schedules[0].data = schedule;
}
