#include <xc.h>
#include <stdint.h>
#include <string.h>

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

static void onDateChanged(const struct Event *event);
static void onTimeChanged(const struct Event *event);

static uint8_t dateDiscriminator;
static struct FarScheduleWithFlags schedules[MAX_SCHEDULES];
static struct FarScheduleWithFlags *noMoreSchedules =
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

	dateDiscriminator = 1;
	memset(&schedules, 0, sizeof(schedules));
}

static void onDateChanged(const struct Event *event)
{
	dateDiscriminator++;
}

static void onTimeChanged(const struct Event *event)
{
	const struct TimeChanged *args = (const struct TimeChanged *) event->args;
	for (struct FarScheduleWithFlags *schedule = schedules; schedule != noMoreSchedules; schedule++)
	{
		if (
			schedule->dateDiscriminator == dateDiscriminator &&
			schedule->data->time.hour == args->now->hour &&
			schedule->data->time.minute == args->now->minute)
		{
			eventPublish(schedule->data->eventType, schedule->data->eventArgs);
			schedule->data = (const struct FarSchedule *) 0;
		}
	}
}

void farSchedulerAdd(const struct FarSchedule *schedule)
{
	for (struct FarScheduleWithFlags *dest = schedules; dest != noMoreSchedules; dest++)
	{
		if (!dest->data)
		{
			dest->dateDiscriminator = dateDiscriminator;
			dest->data = schedule;
			return;
		}
	}
}

void farSchedulerRemove(const struct FarSchedule *schedule)
{
	for (struct FarScheduleWithFlags *dest = schedules; dest != noMoreSchedules; dest++)
	{
		if (dest->data == schedule)
			dest->data = (const struct FarSchedule *) 0;
	}
}
