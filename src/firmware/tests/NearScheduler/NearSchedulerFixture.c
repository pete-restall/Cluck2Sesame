#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "NearScheduler.h"

#include "NonDeterminism.h"
#include "NearSchedulerFixture.h"

const struct NearSchedule dummySchedule =
{
	.ticks = 0,
	.handler = &dummyHandler,
	.state = (void *) 0
};

static const struct EventSubscription *onWokenFromSleep;
static struct Event wokenFromSleepEvent;

static uint8_t numberOfHandlerCalls;
static const struct NearSchedule *handlerSchedules[16];
static const struct NearSchedule **handlerScheduleWrptr;
static const struct NearSchedule **handlerScheduleRdptr;

void setUp(void)
{
	onWokenFromSleep = (struct EventSubscription *) 0;
	nearSchedulerInitialise();
	numberOfHandlerCalls = 0;
	handlerScheduleWrptr = &handlerSchedules[0];
	handlerScheduleRdptr = handlerScheduleWrptr;

	// HACK: Simulator does not respect NCO clock source, so NCO1ACC*
	// manipulation does not produce the correct values if there is an
	// increment.
	NCO1INCU = 0;
	NCO1INCH = 0;
	NCO1INCL = 0;
}

void tearDown(void)
{
}

void tick(void)
{
	PIR7bits.NCO1IF = 1;
	wokenFromSleep();
}

void wokenFromSleep(void)
{
	if (onWokenFromSleep && onWokenFromSleep->handler)
	{
		onWokenFromSleep->handler(&wokenFromSleepEvent);
	}
	else
	{
		TEST_FAIL_MESSAGE("No WokenFromSleep handler");
	}
}

void assertNoHandlersCalled(void)
{
	TEST_ASSERT_EQUAL_UINT8(0, numberOfHandlerCalls);
}

void assertHandlerCalledOnceWith(const struct NearSchedule *const schedule)
{
	assertHandlerCalledTimes(1);
	assertHandlerCalledWith(schedule);
}

void assertHandlerCalledTimes(uint8_t times)
{
	TEST_ASSERT_EQUAL_UINT8(times, numberOfHandlerCalls);
}

void assertHandlerCalledWith(const struct NearSchedule *const schedule)
{
	if (handlerScheduleRdptr == handlerScheduleWrptr)
	{
		TEST_FAIL_MESSAGE("Not enough calls");
	}

	const struct NearSchedule *actual = *(handlerScheduleRdptr++);
	TEST_ASSERT_NOT_NULL_MESSAGE(actual, "Null schedule");
	TEST_ASSERT_NOT_EQUAL_MESSAGE(schedule, actual, "Expected copy");
	TEST_ASSERT_EQUAL_MESSAGE(schedule->handler, actual->handler, "Handler");
	TEST_ASSERT_EQUAL_MESSAGE(schedule->state, actual->state, "State");
}

void eventSubscribe(const struct EventSubscription *const subscription)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription, "Subscription");
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription->handler, "Handler");
	TEST_ASSERT_EQUAL_MESSAGE(
		WOKEN_FROM_SLEEP,
		subscription->type,
		"Unexpected type");

	static const struct WokenFromSleep emptyEventArgs = { };
	onWokenFromSleep = subscription;
	wokenFromSleepEvent.type = subscription->type;
	wokenFromSleepEvent.state = subscription->state;
	wokenFromSleepEvent.args = &emptyEventArgs;
}

void spyHandler(const struct NearSchedule *const schedule)
{
	numberOfHandlerCalls++;
	*(handlerScheduleWrptr++) = schedule;
}

void dummyHandler(const struct NearSchedule *const schedule)
{
}
