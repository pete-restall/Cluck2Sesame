#include <xc.h>
#include <unity.h>

#include "Mock_Event.h"
#include "Mock_Clock.h"

#include "SunEvents.h"

#include "../NonDeterminism.h"

TEST_FILE("SunEvents/SunEventsInitialise.c")

static struct Event onDateChangedEvent;
static const struct EventSubscription *onDateChanged;

static const uint8_t dummyLookupTables[300];
const __rom void *const sunriseLookupTable = &dummyLookupTables;
const __rom void *const sunsetLookupTable = &dummyLookupTables;

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
	else
		TEST_FAIL_MESSAGE("Unknown subscription type");
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
	static const struct SunEventsChanged args = { /* TODO ! */ };
	sunEventsInitialise();
	//onDateChangedEvent.args = ...; // TODO !
	eventPublish_Expect(SUN_EVENTS_CHANGED, &args);
	onDateChanged->handler(&onDateChangedEvent);
}
