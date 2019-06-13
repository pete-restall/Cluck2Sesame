#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Event.h"
#include "Mock_Clock.h"
#include "Platform/FarScheduler.h"

#include "../../Fixture.h"
#include "../../NonDeterminism.h"

TEST_FILE("Platform/FarScheduler.c")

static void onScheduled(const struct Event *event);
static void stubScheduleFor(
	struct FarSchedule *schedule,
	uint8_t hour,
	uint8_t minute);

static struct EventSubscription onScheduledSubscription;
static uint8_t onScheduledCalls;
static const void *onScheduledArgs;

void onBeforeTest(void)
{
	eventInitialise();
	farSchedulerInitialise();

	onScheduledCalls = 0;
	onScheduledArgs = (void *) 0;

	onScheduledSubscription.type = anyByteWithMaskSet(0x70);
	onScheduledSubscription.handler = &onScheduled;
	onScheduledSubscription.state = (void *) 0;
	eventSubscribe(&onScheduledSubscription);
}

static void onScheduled(const struct Event *event)
{
	onScheduledArgs = event->args;
	onScheduledCalls++;
}

void onAfterTest(void)
{
}

void test_timeChanged_onPublishedWithMatchingScheduleTimeButAfterDateChanged_expectScheduleIsNotActioned(void)
{
	static const struct DateWithFlags today = { .year = 1, .month = 2, .day = 3 };
	static const struct DateChanged dateChangedArgs = { .today = &today };

	static const struct Time now = { .hour = 4, .minute = 5 };
	static const struct TimeChanged timeChangedArgs = { .now = &now };

	struct FarSchedule schedule;
	stubScheduleFor(&schedule, 4, 5);
	farSchedulerAdd(&schedule);

	eventPublish(DATE_CHANGED, &dateChangedArgs);
	eventPublish(TIME_CHANGED, &timeChangedArgs);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, onScheduledCalls);
}

static void stubScheduleFor(
	struct FarSchedule *schedule,
	uint8_t hour,
	uint8_t minute)
{
	schedule->time.hour = hour;
	schedule->time.minute = minute;
	schedule->eventType = onScheduledSubscription.type;
	schedule->eventArgs = (void *) (int) anyWord();
}

void test_timeChanged_onPublishedWithMatchingScheduleTime_expectScheduleIsActioned(void)
{
	static const struct DateWithFlags today = { .year = 1, .month = 2, .day = 3 };
	static const struct DateChanged dateChangedArgs = { .today = &today };

	static const struct Time now = { .hour = 4, .minute = 5 };
	static const struct TimeChanged timeChangedArgs = { .now = &now };

	struct FarSchedule schedule;
	stubScheduleFor(&schedule, 4, 5);
	farSchedulerAdd(&schedule);

	eventPublish(TIME_CHANGED, &timeChangedArgs);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(1, onScheduledCalls, "Calls");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(schedule.eventArgs, onScheduledArgs, "Args");
}

void test_timeChanged_onPublishedWithNonMatchingHourInTheFuture_expectScheduleIsNotActioned(void)
{
	static const struct DateWithFlags today = { .year = 1, .month = 2, .day = 3 };
	static const struct DateChanged dateChangedArgs = { .today = &today };

	static const struct Time now = { .hour = 4, .minute = 5 };
	static const struct TimeChanged timeChangedArgs = { .now = &now };

	struct FarSchedule schedule;
	stubScheduleFor(&schedule, now.hour + 1, now.minute);
	farSchedulerAdd(&schedule);

	eventPublish(TIME_CHANGED, &timeChangedArgs);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onScheduledCalls, "Calls");
}

void test_timeChanged_onPublishedWithNonMatchingHourInThePast_expectScheduleIsNotActioned(void)
{
	static const struct DateWithFlags today = { .year = 1, .month = 2, .day = 3 };
	static const struct DateChanged dateChangedArgs = { .today = &today };

	static const struct Time now = { .hour = 4, .minute = 5 };
	static const struct TimeChanged timeChangedArgs = { .now = &now };

	struct FarSchedule schedule;
	stubScheduleFor(&schedule, now.hour - 1, now.minute);
	farSchedulerAdd(&schedule);

	eventPublish(TIME_CHANGED, &timeChangedArgs);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onScheduledCalls, "Calls");
}

void test_timeChanged_onPublishedWithNonMatchingMinuteInThePast_expectScheduleIsNotActioned(void)
{
	static const struct DateWithFlags today = { .year = 1, .month = 2, .day = 3 };
	static const struct DateChanged dateChangedArgs = { .today = &today };

	static const struct Time now = { .hour = 4, .minute = 5 };
	static const struct TimeChanged timeChangedArgs = { .now = &now };

	struct FarSchedule schedule;
	stubScheduleFor(&schedule, now.hour, now.minute - 1);
	farSchedulerAdd(&schedule);

	eventPublish(TIME_CHANGED, &timeChangedArgs);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onScheduledCalls, "Calls");
}

void test_timeChanged_onPublishedWithNonMatchingMinuteInTheFuture_expectScheduleIsNotActioned(void)
{
	static const struct DateWithFlags today = { .year = 1, .month = 2, .day = 3 };
	static const struct DateChanged dateChangedArgs = { .today = &today };

	static const struct Time now = { .hour = 4, .minute = 5 };
	static const struct TimeChanged timeChangedArgs = { .now = &now };

	struct FarSchedule schedule;
	stubScheduleFor(&schedule, now.hour, now.minute + 1);
	farSchedulerAdd(&schedule);

	eventPublish(TIME_CHANGED, &timeChangedArgs);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, onScheduledCalls, "Calls");
}

void test_timeChanged_onPublishedWhenMoreThanOneMatchingSchedule_expectAllSchedulesAreActioned(void)
{
	static const struct DateWithFlags today = { .year = 1, .month = 2, .day = 3 };
	static const struct DateChanged dateChangedArgs = { .today = &today };

	static const struct Time now = { .hour = 4, .minute = 5 };
	static const struct TimeChanged timeChangedArgs = { .now = &now };

	struct FarSchedule schedule;
	stubScheduleFor(&schedule, now.hour, now.minute);
	farSchedulerAdd(&schedule);
	farSchedulerAdd(&schedule);

	eventPublish(TIME_CHANGED, &timeChangedArgs);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(2, onScheduledCalls, "Calls");
}

void test_farSchedulerAdd_calledWhenNoMoreSpace_expectNoSchedulesAreOverwritten(void)
{
	static const struct DateWithFlags today = { .year = 1, .month = 2, .day = 3 };
	static const struct DateChanged dateChangedArgs = { .today = &today };

	static struct Time now = { .hour = 4, .minute = 5 };
	static const struct TimeChanged timeChangedArgs = { .now = &now };

	static struct FarSchedule schedules[33];
	for (uint8_t i = 0; i < 33; i++)
	{
		stubScheduleFor(&schedules[i], now.hour, now.minute + i);
		farSchedulerAdd(&schedules[i]);
	}

	for (uint8_t i = 0; i < 32; i++)
	{
		eventPublish(TIME_CHANGED, &timeChangedArgs);
		dispatchAllEvents();
		now.minute++;
	}

	TEST_ASSERT_EQUAL_UINT8_MESSAGE(32, onScheduledCalls, "(1)");

	eventPublish(TIME_CHANGED, &timeChangedArgs);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(32, onScheduledCalls, "(2)");
}

void test_farSchedulerAdd_calledWhenNoMoreSpaceButSomeSchedulesHaveBeenActioned_expectActionedSchedulesAreOverwritten(void)
{
	static const struct DateWithFlags today = { .year = 1, .month = 2, .day = 3 };
	static const struct DateChanged dateChangedArgs = { .today = &today };

	static struct Time now = { .hour = 4, .minute = 5 };
	static const struct TimeChanged timeChangedArgs = { .now = &now };

	static struct FarSchedule schedules[33];
	stubScheduleFor(&schedules[32], now.hour, now.minute);
	farSchedulerAdd(&schedules[32]);
	eventPublish(TIME_CHANGED, &timeChangedArgs);
	dispatchAllEvents();
	now.minute++;

	for (uint8_t i = 0; i < 32; i++)
	{
		stubScheduleFor(&schedules[i], now.hour, now.minute + i);
		farSchedulerAdd(&schedules[i]);
	}

	for (uint8_t i = 0; i < 32; i++)
	{
		eventPublish(TIME_CHANGED, &timeChangedArgs);
		dispatchAllEvents();
		now.minute++;
	}

	TEST_ASSERT_EQUAL_UINT8(33, onScheduledCalls);
}

void test_farSchedulerRemove_called_expectScheduleIsNotActioned(void)
{
	static const struct DateWithFlags today = { .year = 1, .month = 2, .day = 3 };
	static const struct DateChanged dateChangedArgs = { .today = &today };

	static const struct Time now = { .hour = 4, .minute = 5 };
	static const struct TimeChanged timeChangedArgs = { .now = &now };

	struct FarSchedule schedule;
	stubScheduleFor(&schedule, 4, 5);
	farSchedulerAdd(&schedule);
	farSchedulerRemove(&schedule);

	eventPublish(TIME_CHANGED, &timeChangedArgs);
	dispatchAllEvents();
	TEST_ASSERT_EQUAL_UINT8(0, onScheduledCalls);
}
