#include <xc.h>
#include <stdint.h>
#include <unity.h>

#include "Platform/Mock_Event.h"
#include "Platform/Mock_Clock.h"

#include "SunEvents.h"

#include "../Fixture.h"
#include "../NonDeterminism.h"

TEST_FILE("SunEvents/SunEventsInitialise.c")
TEST_FILE("SunEvents/SunEventsCalculate.c")

static void ensureEventHandlerDoesNotGetOmittedByTheCompiler(void);

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

void onBeforeTest(void)
{
	eventSubscribe_StubWithCallback(&eventSubscribeStub);
	onDateChanged = (const struct EventSubscription *) 0;
}

void onAfterTest(void)
{
}

void test_sunEventsInitialise_called_expectSubscriptionToDateChanged(void)
{
	sunEventsInitialise();
	ensureEventHandlerDoesNotGetOmittedByTheCompiler();
	TEST_ASSERT_NOT_NULL(onDateChanged);
}

static void ensureEventHandlerDoesNotGetOmittedByTheCompiler(void)
{
	static volatile uint8_t dummy = 0;
	if (onDateChanged && dummy)
		onDateChanged->handler(&onDateChangedEvent);
}
