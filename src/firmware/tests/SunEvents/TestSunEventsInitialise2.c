#include <xc.h>
#include <unity.h>

#include "Mock_Event.h"
#include "Mock_Clock.h"

#include "SunEvents.h"

#include "../NonDeterminism.h"

TEST_FILE("SunEvents/SunEventsInitialise.c")
TEST_FILE("SunEvents/SunEventsCalculate.c")

static struct Event onDateChangedEvent;
static const struct EventSubscription *onDateChanged;

static void eventSubscribeStub(
	const struct EventSubscription *const subscription,
	int numCalls)
{
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription, "Null subscription");
	TEST_ASSERT_NOT_NULL_MESSAGE(subscription->handler, "Null handler");
	if (subscription->type == DATE_CHANGED)
	{
		onDateChanged = subscription;
		onDateChangedEvent.type = subscription->type;
		onDateChangedEvent.state = subscription->state;
		onDateChangedEvent.args = (void *) 0;
	}
}

void setUp(void)
{
	eventSubscribe_StubWithCallback(&eventSubscribeStub);
	onDateChanged = (const struct EventSubscription *) 0;
}

void tearDown(void)
{
}

void test_sunEventsInitialise_called_expectSubscriptionToDateChanged(void)
{
	sunEventsInitialise();
	TEST_ASSERT_NOT_NULL(onDateChanged);
}

void test_onDateChanged_ExpectSunEventsChangedIsPublishedWithCalculatedSunEventTimes(void)
{
	static const struct Date today =
	{
		.day = 0, .month = 0, .year = 0,
		.dayOfYear = 0,
		.flags = { .isLeapYear = 0, .isDaylightSavings = 0 }
	};

	static const struct DateChanged dateChanged =
	{
		.today = &today
	};

	sunEventsInitialise();
	onDateChangedEvent.args = &dateChanged;
	eventPublish_Expect(SUN_EVENTS_CHANGED, NULL);
	eventPublish_IgnoreArg_args();
	onDateChanged->handler(&onDateChangedEvent);
}
