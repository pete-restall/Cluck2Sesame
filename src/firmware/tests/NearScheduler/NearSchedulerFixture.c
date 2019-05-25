#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "NearScheduler.h"

#include "NearSchedulerFixture.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

const struct NearSchedule dummySchedule =
{
	.ticks = 0,
	.handler = &dummyHandler,
	.state = (void *) 0
};

static const struct EventSubscription *onWokenFromSleep;
static struct Event wokenFromSleepEvent;

static uint8_t numberOfHandlerCalls;
static const void *handlerSchedules[16];
static const void **handlerStateWrptr;
static const void **handlerStateRdptr;

void onBeforeTest(void)
{
	onWokenFromSleep = (struct EventSubscription *) 0;
	nearSchedulerInitialise();
	numberOfHandlerCalls = 0;
	handlerStateWrptr = &handlerSchedules[0];
	handlerStateRdptr = handlerStateWrptr;

	// HACK: Simulator does not respect NCO clock source, so NCO1ACC*
	// manipulation does not produce the correct values if there is an
	// increment.
	NCO1INCU = 0;
	NCO1INCH = 0;
	NCO1INCL = 0;
}

void onAfterTest(void)
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

void assertHandlerCalledOnceWith(const void *const state)
{
	assertHandlerCalledTimes(1);
	assertHandlerCalledWith(state);
}

void assertHandlerCalledTimes(uint8_t times)
{
	TEST_ASSERT_EQUAL_UINT8(times, numberOfHandlerCalls);
}

void assertHandlerCalledWith(const void *const state)
{
	if (handlerStateRdptr == handlerStateWrptr)
	{
		TEST_FAIL_MESSAGE("Not enough calls");
	}

	const void *actual = *(handlerStateRdptr++);
	TEST_ASSERT_EQUAL_PTR(state, actual);
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

void spyHandler(void *const state)
{
	numberOfHandlerCalls++;
	*(handlerStateWrptr++) = state;
}

void dummyHandler(void *const state)
{
}
