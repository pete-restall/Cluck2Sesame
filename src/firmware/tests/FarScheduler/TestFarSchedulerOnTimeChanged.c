#include <xc.h>
#include <unity.h>

#include "Event.h"
#include "Mock_Clock.h"
#include "FarScheduler.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("FarScheduler.c")

static void onScheduled(const struct Event *const event);
static void stubScheduleFor(
	struct FarSchedule *schedule,
	uint8_t hour,
	uint8_t minute);

static void dispatchAllEvents(void);

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

static void onScheduled(const struct Event *const event)
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

static void dispatchAllEvents(void)
{
	while (eventDispatchNext())
		;;
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
